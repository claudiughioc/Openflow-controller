#include <stdlib.h>
#include <stdio.h>

#include "trema.h"

#define SWITCH_NUMBER		3

struct switch_t {
	int next_port;
	char host_mac[6];
};

struct switch_t sws[SWITCH_NUMBER];

/* Generate host mac address */
static inline void host_mac(int i, char *mac)
{
	int j;
	for (j = 0; j < 5; j++)
		mac[j] = 0;
	mac[5] = i;
}

/* Print mac address */
static inline void print_mac(char *mac)
{
	int i = 0;
	for (i = 0; i < 6; i++)
		printf("%x, ", (int)mac[i]);
	printf("\n");
}

/* Handler for switch connected event */
static void switch_connected(uint64_t datapath_id, void *user_data)
{
	printf("A switch has connected switch %d\n", (int)datapath_id);
	if (datapath_id > SWITCH_NUMBER) {
		printf("Unsuported switch\n");
		exit(0);
	}

	/* Set switch specific information */
	sws[datapath_id - 1].next_port = 2;
	host_mac(datapath_id, sws[datapath_id - 1].host_mac);
}

/* Install rule to forward all the packets unrelated to the switch's
 * host */
static void packet_forward(int sw, char *src_mac, char *dst_mac, int in_port)
{
	int res = 0, port;
	openflow_actions *actions;
	struct ofp_match match;
	buffer *buff;

	/* Find the number of the other port */
	switch (in_port) {
	case 2:
		port = 3;
		break;
	case 3:
		port = 2;
		break;
	default:
		printf("Inport is %d, this shouldn't be handled here\n", in_port);
	}

	printf("Must forward this to port %d\n", port);

	/* Install rule to forward flow on the next port */
	memset(&match, 0, sizeof(struct ofp_match));
	match.wildcards = OFPFW_ALL;
	match.wildcards ^= OFPFW_DL_DST;
	match.wildcards ^= OFPFW_DL_SRC;
	memcpy(match.dl_dst, dst_mac, 6);
	memcpy(match.dl_src, src_mac, 6);

	actions = create_actions();
	append_action_output(actions, port, 0);

	buff = create_flow_mod(get_transaction_id(), match, get_cookie(),
			OFPFC_ADD, OFP_FLOW_PERMANENT, OFP_FLOW_PERMANENT,
			0xffff, -1, OFPP_NONE, OFPFF_SEND_FLOW_REM, actions);
	if (!(res = send_openflow_message(sw, buff)))
		printf("Error installing alternate rule, res %d\n", res);

	delete_actions(actions);
	free_buffer(buff);

}

/* Install rule to forward flow to the host */
static void packet_to_host(int sw, char *src_mac, char *dst_mac, int in_port)
{
	int res = 0;
	openflow_actions *actions;
	struct ofp_match match;
	buffer *buff;

	printf("Must forward this to my host\n");

	/* Install rule to forward flow on the switch's host */
	memset(&match, 0, sizeof(struct ofp_match));
	match.wildcards = OFPFW_ALL;
	match.wildcards ^= OFPFW_DL_DST;
	memcpy(match.dl_dst, dst_mac, 6);

	actions = create_actions();
	append_action_output(actions, 1, 0);

	buff = create_flow_mod(get_transaction_id(), match, get_cookie(),
			OFPFC_ADD, OFP_FLOW_PERMANENT, OFP_FLOW_PERMANENT,
			0xffff, -1, OFPP_NONE, OFPFF_SEND_FLOW_REM, actions);
	if (!(res = send_openflow_message(sw, buff)))
		printf("Error installing to host rule, res %d\n", res);

	delete_actions(actions);
	free_buffer(buff);


	/* Install rule to forward packets back to the sender */
	printf("Also install rule to go back through %d\n", in_port);
	memset(&match, 0, sizeof(struct ofp_match));
	match.wildcards = OFPFW_ALL;
	match.wildcards ^= OFPFW_DL_DST;
	match.wildcards ^= OFPFW_DL_SRC;
	memcpy(match.dl_dst, src_mac, 6);
	memcpy(match.dl_src, dst_mac, 6);

	actions = create_actions();
	append_action_output(actions, in_port, 0);

	buff = create_flow_mod(get_transaction_id(), match, get_cookie(),
			OFPFC_ADD, OFP_FLOW_PERMANENT, OFP_FLOW_PERMANENT,
			0xffff, -1, OFPP_NONE, OFPFF_SEND_FLOW_REM, actions);
	if (!(res = send_openflow_message(sw, buff)))
		printf("Error installing back from host rule, res %d\n", res);

	delete_actions(actions);
	free_buffer(buff);
}

/* Install rule to forward flow on the next port */
static void packet_from_host(int sw, char *src_mac, char *dst_mac)
{
	int res = 0, next_port = sws[sw - 1].next_port;
	openflow_actions *actions;
	struct ofp_match match;
	buffer *buff;

	printf("Must alternate this to port %d\n", next_port);

	/* Set the future next port */
	if (sws[sw - 1].next_port == 2)
		sws[sw - 1].next_port = 3;
	else
		sws[sw - 1].next_port = 2;


	/* Install rule to forward flow on the next port */
	memset(&match, 0, sizeof(struct ofp_match));
	match.wildcards = OFPFW_ALL;
	match.wildcards ^= OFPFW_DL_SRC;
	memcpy(match.dl_src, src_mac, 6);

	actions = create_actions();
	append_action_output(actions, next_port, 0);

	buff = create_flow_mod(get_transaction_id(), match, get_cookie(),
			OFPFC_ADD, OFP_FLOW_PERMANENT, OFP_FLOW_PERMANENT,
			0xffff, -1, OFPP_NONE, OFPFF_SEND_FLOW_REM, actions);
	if (!(res = send_openflow_message(sw, buff)))
		printf("Error installing alternate rule, res %d\n", res);

	delete_actions(actions);
	free_buffer(buff);


	/* Install rule to forward packets back to the host */
	printf("Also install rule to go back to host\n");
	memset(&match, 0, sizeof(struct ofp_match));
	match.wildcards = OFPFW_ALL;
	match.wildcards ^= OFPFW_DL_DST;
	match.wildcards ^= OFPFW_DL_SRC;
	memcpy(match.dl_dst, src_mac, 6);
	memcpy(match.dl_src, dst_mac, 6);

	actions = create_actions();
	append_action_output(actions, 1, 0);

	buff = create_flow_mod(get_transaction_id(), match, get_cookie(),
			OFPFC_ADD, OFP_FLOW_PERMANENT, OFP_FLOW_PERMANENT,
			0xffff, -1, OFPP_NONE, OFPFF_SEND_FLOW_REM, actions);
	if (!(res = send_openflow_message(sw, buff)))
		printf("Error installing back from host rule, res %d\n", res);

	delete_actions(actions);
	free_buffer(buff);
}

/* Handler for packet in event */
void packet_in_h(uint64_t datapath_id,
		uint32_t transaction_id,
		uint32_t buffer_id,
		uint16_t total_len,
		uint16_t in_port,
		uint8_t reason,
		const buffer *data,
		void *user_data)
{
	char src_mac[6], dst_mac[6];
	int res = 0;

	/* Get src and dest max addresses */
	packet_info packet_info = get_packet_info(data);
	memcpy(src_mac, packet_info.eth_macsa, OFP_ETH_ALEN);
	memcpy(dst_mac, packet_info.eth_macda, OFP_ETH_ALEN);

	printf("\nReceived packet on s%dfrom port %d with info:\n",
			(int)datapath_id, in_port);
	print_mac(src_mac);
	print_mac(dst_mac);


	/* Case 1:
	 * Switch receives a packet from its host
	 * Forward it alternatively on the two exit ports */
	if (!memcmp(src_mac, sws[datapath_id - 1].host_mac, 6)) {
		packet_from_host((int)datapath_id, src_mac, dst_mac);
		return;
	}


	/* Case 2:
	 * Switch receives a packet for its host
	 * Set a rule to forward all packets to host */
	if (!memcmp(dst_mac, sws[datapath_id - 1].host_mac, 6)) {
		packet_to_host((int)datapath_id, src_mac, dst_mac, in_port);
		return;
	}


	/* Case 3:
	 * Switch receives a packet which is not related to the host
	 * Forward it on the other free port */
	packet_forward((int)datapath_id, src_mac, dst_mac, in_port);
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
