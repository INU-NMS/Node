#include "dot_util.h"
#include "RadioEvent.h"
#include "Serial.h"
#include "node_util.h"
#include <string.h>

#define NODE_APP_VERSION 0.1
#define LORA_INFO(str, ...)	printf("[LORA_INFO] " str "\r\n", ##__VA_ARGS__)

static uint8_t network_id[] = { 0x11, 0x12, 0x13, 0x14, 0x15, 0x16, 0x17, 0x18 };
static uint8_t network_key[] = { 1, 2, 3, 4, 5, 6, 7, 8, 9, 16, 17, 18, 19, 20, 21, 22 };
static bool frequency_sub_band = 0;
static bool public_network = true;
static uint8_t ack = 1;
static bool adr = false;

#define CMD_GET 0
#define CMD_SET 1
#define CMD_SND 2

mDot* dot = NULL;
lora::ChannelPlan* plan = NULL;
Serial pc(USBTX, USBRX);
char buf[1024];

std::vector<uint8_t> payload;

uint8_t parse(char* mes, char* var, uint8_t* val) {
	if(strcmp(mes, "send\r\n") == 0) return CMD_SND;
	char* tmp = strtok(mes, "=");
	sprintf(var, "%s", tmp);
	tmp = strtok(NULL, "=");
	if(tmp == NULL) return CMD_GET;
	*val = (uint8_t)atoi(tmp);
	return CMD_SET;
}

void joinNetwork() {
	dot->joinNetwork();
	printf("status=%s\r\n", dot->getNetworkJoinStatus() ? "true" : "false");
}

void process(char* mes) {
	char var[32];
	uint8_t val = 0;

	uint8_t command_type = parse(mes, var, &val);

	if(command_type == CMD_GET) {
		if(strcmp(var, "power\r\n") == 0) printf("power=%lu\r\n", dot->getTxPower());
		if(strcmp(var, "datarate\r\n") == 0) printf("datarate=%u\r\n", dot->getTxDataRate());
		if(strcmp(var, "length\r\n") == 0) printf("length=%u\r\n", payload.size());
		if(strcmp(var, "info\r\n") == 0) displayInformation();
		if(strcmp(var, "join\r\n") == 0) joinNetwork();
		if(strcmp(var, "eui\r\n") == 0) printf("%s\r\n", mts::Text::bin2hexString(dot->getDeviceId()).c_str());
		if(strcmp(var, "status\r\n") == 0) printf("status=%s\r\n", dot->getNetworkJoinStatus() ? "true" : "false");
		if(strcmp(var, "sending\r\n") == 0) printf("sending=%d\r\n", isSending());
	}
	if(command_type == CMD_SET) {
		if(strcmp(var, "power") == 0) setPower(val);
		if(strcmp(var, "datarate") == 0) setDataRate(val);
		if(strcmp(var, "length") == 0) setPacketLength(payload, val);
	}
	if(command_type == CMD_SND) {
		send(payload);
	}
}

void loop() {
	printf("Ready to process\r\n");
	while(true) {
		pc.gets(buf, sizeof buf);
		process(buf);
	}
}

int main() {
	RadioEvent events;
    pc.baud(115200);
	mts::MTSLog::setLogLevel(mts::MTSLog::NONE_LEVEL);
	
	LORA_INFO("nodeApp with mbed-os lib %d", MBED_LIBRARY_VERSION);
	LORA_INFO("Dev Addr: %s", mts::Text::bin2hexString(dot->getNetworkAddress()).c_str());

	plan = new lora::ChannelPlan_KR920();
	dot = mDot::getInstance(plan);
	assert(dot);
	dot->setEvents(&events);
	dot->setJoinMode(mDot::OTA);
	update_ota_config_id_key(network_id, network_key, frequency_sub_band, public_network, ack);
	dot->setAdr(adr);
	dot->setTxPower(0);
	dot->setTxDataRate(0);
	dot->setDisableDutyCycle(true);
	dot->saveConfig();
	joinNetwork();

	LORA_INFO("Dev Addr: %s", mts::Text::bin2hexString(dot->getNetworkAddress()).c_str());
	LORA_INFO("Net Session Key: %s", mts::Text::bin2hexString(dot->getNetworkSessionKey()).c_str());
	LORA_INFO("App Session Key: %s", mts::Text::bin2hexString(dot->getDataSessionKey()).c_str());

	loop();

	return 0;
}
