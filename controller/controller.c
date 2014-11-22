#include <stdlib.h>
#include <stdio.h>

#include "trema.h"

static void switch_connected(uint64_t datapath_id, void *user_data)
{
	printf("A switch has connected \n");
}

void packet_in_h(uint64_t datapath_id,
		uint32_t transaction_id,
		uint32_t buffer_id,
		uint16_t total_len,
		uint16_t in_port,
		uint8_t reason,
		const buffer *data,
		void *user_data)
{

}

int main(int argc, char **argv)
{
	init_trema(&argc, &argv);

	printf("Waiting for a switch to register\n");
	set_switch_ready_handler(switch_connected, NULL);
	set_packet_in_handler(packet_in_h, NULL);

	start_trema();

	return EXIT_SUCCESS;
}
