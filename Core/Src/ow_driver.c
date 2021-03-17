/*
 * DS2480_driver.c
 *
 *  Created on: 20 дек. 2020 г.
 *      Author: Nikon
 */
#include <ow_driver.h>
#include <ow_resources.h>

#define SCRATCH_PAD_SIZE 9

#define RETURN_IF_ERROR(func, error) if((error = func) != OK) return error;

#define INC_PTR(ptr) (ptr = (ptr + 1) % BUFFER_SIZE)
#define ADD_PTR(ptr, len) (ptr = (ptr + len) % BUFFER_SIZE)
#define LEN(ptr1, ptr2) ((ptr1) < (ptr2) ? ((ptr2) - (ptr1)) : ((ptr2) + BUFFER_SIZE - (ptr1)))

static uint8_t buf[BUFFER_SIZE];
static int16_t write_ptr = 0;
static int16_t read_ptr = 0;
static uint8_t is_ow_in_cmd_mod = 1;

static int16_t start_write_ptr = 0;
osThreadId listener_id;
static Command_t *current_command = NULL;

static UART_HandleTypeDef *uart;
static uint16_t timeout;

static uint8_t search_request[16];

static OW_ERROR_E Detect(void);
static OW_ERROR_E Reset(void);
static OW_ERROR_E ChangeBaudToLow(void);
static OW_ERROR_E SetPullUpDuration(uint8_t duration);

static int16_t ConvertTemp(uint8_t hi, uint8_t lo);

static OW_ERROR_E Search(Sensor_t *sensors[], uint8_t size);
static void CleanUpSearchFrame(uint8_t last_discrepancy);
static OW_ERROR_E SelectSensor(Sensor_t *sensor);
static OW_ERROR_E ConvertT(void);
static OW_ERROR_E ConvertTPullUp(void);
static OW_ERROR_E ReadScratchpad(uint16_t *ptr);
static uint8_t CheckOneWireCRC(uint16_t ptr, uint16_t len, uint8_t crc);
static OW_ERROR_E WriteScratchpad();

static OW_ERROR_E ToCMDMode(void);
static OW_ERROR_E ToDataMode(void);
static OW_ERROR_E RunByteCommand(uint8_t command, uint16_t *ptr);
static OW_ERROR_E RunByteCommandWithResponce(uint8_t command, uint16_t *ptr);
static OW_ERROR_E RunCommand(Command_t *command, uint16_t *ptr);

void OWDriverInit(UART_HandleTypeDef *_uart, uint16_t ms_timeout) {
	timeout = ms_timeout;
	uart = _uart;
}

OW_ERROR_E OWStart(Sensor_t *sensors[], uint8_t size) {
	is_ow_in_cmd_mod = 1;
	OW_ERROR_E error;

	RETURN_IF_ERROR(Detect(), error);
	RETURN_IF_ERROR(ChangeBaudToLow(), error);
	RETURN_IF_ERROR(Reset(), error);
	RETURN_IF_ERROR(Search(sensors, size), error);
	RETURN_IF_ERROR(SetPullUpDuration(OW_PULLAP_DUR_1048), error);

	return OK;
}

OW_ERROR_E OWMeasureTemperature(Sensor_t *sensor, int16_t *temperature) {
	OW_ERROR_E error;
	RETURN_IF_ERROR(Reset(), error);

	RETURN_IF_ERROR(SelectSensor(sensor), error);
	RETURN_IF_ERROR(ConvertTPullUp(), error);

	RETURN_IF_ERROR(Reset(), error);
	RETURN_IF_ERROR(SelectSensor(sensor), error);

	uint16_t ptr;
	RETURN_IF_ERROR(ReadScratchpad(&ptr), error);

	uint8_t lo = buf[INC_PTR(ptr)];
	uint8_t hi = buf[INC_PTR(ptr)];
	*temperature = ConvertTemp(hi, lo);

	return OK;
}

static int16_t ConvertTemp(uint8_t hi, uint8_t lo) {
	int16_t result = 100 * ((hi << 8) | lo) / 16;

	if (hi & 0x80)
		result *= -1;

	return result;
}

static OW_ERROR_E Detect(void) {
	uint16_t ptr;
	RunByteCommand(OW_SYNC, &ptr);
	osDelay(2);
	return RunCommand(&detect_command, &ptr);
}

static OW_ERROR_E ChangeBaudToLow(void) {
	ToCMDMode();
	uint16_t ptr;
	OW_ERROR_E error;
	RETURN_IF_ERROR(RunCommand(&change_baud_low_command, &ptr), error);
	INC_PTR(ptr);
	uint16_t response = buf[INC_PTR(ptr)];

	if (response == 0)
		return OK;

	return UNKONWN_ERROR;
}

static OW_ERROR_E SetPullUpDuration(uint8_t duration) {
	uint16_t ptr;
	return RunByteCommandWithResponce(duration, &ptr);
}

static OW_ERROR_E Reset(void) {
	ToCMDMode();
	uint16_t ptr;
	OW_ERROR_E error;
	RETURN_IF_ERROR(RunByteCommandWithResponce(OW_RESET, &ptr), error);

	uint16_t response = buf[INC_PTR(ptr)];

	if ((response & 0x3) == 0)
		return SHORT_CIRCUIT_ERROR;
	if ((response & 0x3) == 3)
		return NO_SENSOR_ERROR;

	return OK;
}

OW_ERROR_E Search(Sensor_t *sensors[], uint8_t size) {
	for (uint8_t i = 5; i < 21; i++)
		search_frame[i] = 0;

	uint8_t last_discrepancy = 0;

	uint16_t ptr;
	OW_ERROR_E error;
	uint8_t addr_byte = 0;
	uint8_t responce_byte;
	uint8_t sensor_idx = 0;
	uint8_t addres[8];

	do {
		RETURN_IF_ERROR(Reset(), error);
		RETURN_IF_ERROR(RunCommand(&search_command, &ptr), error);
		INC_PTR(ptr);

		for (uint8_t i = 0; i < 8; i++) {
			sensors[sensor_idx]->addr[i] = 0;
			addres[i] = 0;
		}

		for (uint8_t bit = 0; bit < 64; bit++) {
			if (bit % 4 == 0)
				responce_byte = buf[INC_PTR(ptr)];

			addr_byte = bit / 8;

			uint8_t discrepancy_bit = responce_byte & 0x1;
			uint8_t rom_id_bit = (responce_byte >> 1) & 0x1;
			responce_byte = responce_byte >> 2;

			if (discrepancy_bit && !rom_id_bit)  // branching
				last_discrepancy = bit;

			addres[addr_byte] |= rom_id_bit << (bit % 8);
			search_frame[5 + bit / 4] |= rom_id_bit << (2 * (bit % 4) + 1);
		}

		for (uint8_t i = 0; i < 8; i++)
			sensors[sensor_idx]->addr[i] = addres[i];

		sensor_idx++;
		if (sensor_idx > size)
			return OK;

		if (last_discrepancy) {
			search_frame[5 + last_discrepancy / 4] |= 1
					<< (2 * (last_discrepancy % 4) + 1);
			CleanUpSearchFrame(last_discrepancy);
			last_discrepancy = 0;
		} else {
			break;
		}
	} while (1);

	return Reset();
}

static void CleanUpSearchFrame(uint8_t last_discrepancy) {
	uint8_t rest_ones_in_index = 0xFF >> (8 - 2 * ((last_discrepancy) % 4 + 1));
	search_frame[5 + last_discrepancy / 4] &= rest_ones_in_index;

	for (uint8_t idx = 6 + last_discrepancy / 4; idx < 21; idx++) {
		search_frame[idx] = 0;
	}
}

static OW_ERROR_E SelectSensor(Sensor_t *sensor) {
	OW_ERROR_E error;
	uint16_t ptr;

	ToDataMode();

	if (sensor == NULL) {
		RETURN_IF_ERROR(RunByteCommandWithResponce(OW_SKIP_ROM, &ptr), error);
	}

	RETURN_IF_ERROR(RunByteCommandWithResponce(OW_MATHC_ROM, &ptr), error);
	Command_t rom_code_command = { .request_frame = &sensor->addr,
			.request_size = 8, .responce_size = 8 };

	RETURN_IF_ERROR(RunCommand(&rom_code_command, &ptr), error);
	return OK;
}

static OW_ERROR_E ConvertT(void) {
	ToDataMode();
	uint16_t ptr;
	return RunByteCommandWithResponce(OW_CONVERT_T, &ptr);
}

static OW_ERROR_E ConvertTPullUp(void) {
	ToCMDMode();
	uint16_t ptr;
	OW_ERROR_E error = RunCommand(&convert_t_pull_up_command, &ptr);
	OW_ERROR_E error2 = RunCommand(&convert_t_pull_up_second_command, &ptr);

	return error;
}

static OW_ERROR_E ReadScratchpad(uint16_t *ptr) {
	ToDataMode();
	OW_ERROR_E error = RunCommand(&read_scratchpad_command, ptr);

	uint16_t scratchpad_ptr = INC_PTR(*ptr);
	uint8_t new_ptr = *ptr;
	uint8_t crc = buf[ADD_PTR(new_ptr, SCRATCH_PAD_SIZE)];

	if (!CheckOneWireCRC(scratchpad_ptr, SCRATCH_PAD_SIZE - 1, crc))
		return CHECK_CRC_ERROR;

	return error;
}

static uint8_t CheckOneWireCRC(uint16_t ptr, uint16_t len, uint8_t crc) {
	uint8_t calc_crc = 0;
	for (uint8_t i = 0; i < len; i++)
		calc_crc = crc_table[calc_crc ^ buf[INC_PTR(ptr)]];

	return calc_crc == crc;
}

static OW_ERROR_E WriteScratchpad() {
	ToDataMode();
	uint16_t ptr;
	return RunCommand(&write_scratchpad_command, &ptr);
}

static OW_ERROR_E ToCMDMode(void) {
	uint16_t p;
	if (is_ow_in_cmd_mod) {
		return OK;
	} else {
		is_ow_in_cmd_mod = 1;
		return RunByteCommand(OW_TO_CMD, &p);
	}

}

static OW_ERROR_E ToDataMode(void) {
	uint16_t p;
	if (!is_ow_in_cmd_mod) {
		return OK;
	} else {
		is_ow_in_cmd_mod = 0;
		return RunByteCommand(OW_TO_DATA, &p);
	}
}

static OW_ERROR_E RunByteCommand(uint8_t command, uint16_t *ptr) {
	*volatile_one_byte_frame = command;
	return RunCommand(&volatile_one_byte_command, ptr);
}

static OW_ERROR_E RunByteCommandWithResponce(uint8_t command, uint16_t *ptr) {
	*volatile_one_byte_frame = command;
	return RunCommand(&volatile_one_byte_command_with_responce, ptr);
}

static OW_ERROR_E RunCommand(Command_t *command, uint16_t *ptr) {
	osEvent evt;
	*ptr = start_write_ptr = write_ptr;
	current_command = command;
	listener_id = osThreadGetId();

	HAL_UART_Transmit(uart, command->request_frame, command->request_size,
			HAL_MAX_DELAY);

	if (command->responce_size == 0)
		return OK;

	evt = osSignalWait(0x01, timeout);
	//osSignalClear(listener_id, 0x01);
	current_command = NULL;

	if (evt.status == osEventTimeout)
		return TIMEOUT_ERROR;
	if (evt.status == osEventSignal)
		return OK;

	return UNKONWN_ERROR;
}

void StartUartReadTask(void const *argument) {
	osEvent evt;
	uint8_t data;

	for (;;) {
		evt = osMessageGet(UartMessageQueueHandle, osWaitForever); // wait for message
		if (evt.status == osEventMessage) {
			data = (uint8_t) evt.value.p;
			buf[INC_PTR(write_ptr)] = data;

			if (current_command) {
				uint16_t len = LEN(start_write_ptr, write_ptr);
				if (len == current_command->responce_size)
					osSignalSet(listener_id, 0x01);
			}
		}
	}
}
