/*
 * crc_table.h
 *
 *  Created on: 27 янв. 2021 г.
 *      Author: Nikon
 */

#ifndef INC_OW_RESOURCES_H_
#define INC_OW_RESOURCES_H_

typedef __UINT8_TYPE__ uint8_t;

typedef struct Command_t
{
	uint8_t* request_frame;
	uint8_t request_size;
	uint8_t responce_size;
} Command_t;

#define OW_SYNC 0xC1
#define OW_TO_DATA 0xE1
#define OW_TO_CMD 0xE3
#define OW_RESET 0xC5
#define OW_SKIP_ROM 0xCC
#define OW_MATHC_ROM 0x55
#define OW_CONVERT_T 0x44
//0x3B 1048ms,  // 0x39  524ms
#define OW_PULLAP_DUR_524 0x39
#define OW_PULLAP_DUR_1048 0x3B

uint8_t volatile_one_byte_frame[1];
Command_t volatile_one_byte_command =
{
	.request_frame = &volatile_one_byte_frame,
	.request_size = 1,
	.responce_size = 0
};

Command_t volatile_one_byte_command_with_responce =
{
	.request_frame = &volatile_one_byte_frame,
	.request_size = 1,
	.responce_size = 1
};

uint8_t detect_frame[] = {0x17, 0x45, 0x5B, 0x0F, 0x91};
Command_t detect_command =
{
	.request_frame = &detect_frame,
	.request_size = 5,
	.responce_size = 5
};

uint8_t change_baud_low_frame[] = {0x71, 0x0F};
Command_t change_baud_low_command =
{
	.request_frame = &change_baud_low_frame,
	.request_size = 2,
	.responce_size = 2
};

uint8_t search_frame[] = {0xE1, 0xF0, 0xE3, 0xB1, 0xE1,
		0, 0, 0, 0, 0, 0, 0, 0,
		0, 0, 0, 0, 0, 0, 0, 0, 0xE3, 0xA1};
Command_t search_command =
{
	.request_frame = &search_frame,
	.request_size = 23,
	.responce_size = 17
};

uint8_t convert_t_pull_up_frame[] = {0xEF, 0xF1, 0xE1, 0x44};
Command_t convert_t_pull_up_command =
{
	.request_frame = &convert_t_pull_up_frame,
	.request_size = 4,
	.responce_size = 3
};

uint8_t convert_t_pull_up_second_frame[] = {0xE3, 0xED, 0xF1};
Command_t convert_t_pull_up_second_command =
{
	.request_frame = &convert_t_pull_up_second_frame,
	.request_size = 3,
	.responce_size = 1
};

/* deprecated
uint8_t convert_t_pull_up_frame[] = {0x3F, 0x85, 0x95, 0x85, 0x85, 0x85, 0x95, 0x85, 0x87};
Command_t convert_t_pull_up_command =
{
	.request_frame = &convert_t_pull_up_frame,
	.request_size = 9,
	.responce_size = 9
};

uint8_t level_normal_frame[] = {0xF1, 0xED, 0xF1};
Command_t level_normal_command =
{
	.request_frame = &level_normal_frame,
	.request_size = 3,
	.responce_size = 2
};*/

uint8_t write_scratchpad_frame[] = {0x4E, 0x00, 0x00, 0x5F};
Command_t write_scratchpad_command =
{
	.request_frame = &write_scratchpad_frame,
	.request_size = 4,
	.responce_size = 4
};

uint8_t read_scratchpad_frame[] = {0xBE, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF};
Command_t read_scratchpad_command =
{
	.request_frame = &read_scratchpad_frame,
	.request_size = 10,
	.responce_size = 10
};

uint8_t crc_table[] = {
0, 94, 188, 226, 97, 63, 221, 131, 194, 156, 126, 32, 163, 253, 31, 65,
157, 195, 33, 127, 252, 162, 64, 30, 95, 1, 227, 189, 62, 96, 130, 220,
35, 125, 159, 193, 66, 28, 254, 160, 225, 191, 93, 3, 128, 222, 60, 98,
190, 224, 2, 92, 223, 129, 99, 61, 124, 34, 192, 158, 29, 67, 161, 255,
70, 24, 250, 164, 39, 121, 155, 197, 132, 218, 56, 102, 229, 187, 89, 7,
219, 133, 103, 57, 186, 228, 6, 88, 25, 71, 165, 251, 120, 38, 196, 154,
101, 59, 217, 135, 4, 90, 184, 230, 167, 249, 27, 69, 198, 152, 122, 36,
248, 166, 68, 26, 153, 199, 37, 123, 58, 100, 134, 216, 91, 5, 231, 185,
140, 210, 48, 110, 237, 179, 81, 15, 78, 16, 242, 172, 47, 113, 147, 205,
17, 79, 173, 243, 112, 46, 204, 146, 211, 141, 111, 49, 178, 236, 14, 80,
175, 241, 19, 77, 206, 144, 114, 44, 109, 51, 209, 143, 12, 82, 176, 238,
50, 108, 142, 208, 83, 13, 239, 177, 240, 174, 76, 18, 145, 207, 45, 115,
202, 148, 118, 40, 171, 245, 23, 73, 8, 86, 180, 234, 105, 55, 213, 139,
87, 9, 235, 181, 54, 104, 138, 212, 149, 203, 41, 119, 244, 170, 72, 22,
233, 183, 85, 11, 136, 214, 52, 106, 43, 117, 151, 201, 74, 20, 246, 168,
116, 42, 200, 150, 21, 75, 169, 247, 182, 232, 10, 84, 215, 137, 107, 53
};

#endif /* INC_OW_RESOURCES_H_ */
