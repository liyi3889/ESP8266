#include <string.h>
#include <osapi.h>
#include "user_interface.h"
#include "mem.h"
#include "httpd/httpd.h"
#include "espmissingincludes.h"

#include "ip/TcpClient.h"
#include "ip/TcpServer.h"
#include "wifi/WifiUtility.h"
//#include "io/io.h"
#include "gpio.h"
#include "io/key.h"


#define PLUG_KEY_NUM            2

LOCAL struct keys_param keys;
LOCAL struct single_key_param *single_key[PLUG_KEY_NUM];


#define PLUG_KEY_2_IO_MUX     PERIPHS_IO_MUX_MTMS_U
#define PLUG_KEY_2_IO_NUM     2
#define PLUG_KEY_2_IO_FUNC    FUNC_GPIO2

#define PLUG_KEY_0_IO_MUX     PERIPHS_IO_MUX_MTMS_U
#define PLUG_KEY_0_IO_NUM     0
#define PLUG_KEY_0_IO_FUNC    FUNC_GPIO0



//This cgi uses the routines above to connect to a specific access point with the
//given ESSID using the given password.
int ICACHE_FLASH_ATTR cgiPushIndexRequest(HttpdConnData *connData) {
	char text[128];
	httpdFindArg(connData->postBuff, "text", text, sizeof(text));
	httpdPushMessage("/push/listen.push", text);
	espconn_sent(connData->conn, (uint8 *) " ", 1);
	return HTTPD_CGI_DONE;
}

LOCAL void ICACHE_FLASH_ATTR user_plug_short_press(void)
{
   os_printf("User press short !!!!!!!!!!!!!!! \r\n");
   	httpdPushMessage("/push/io.push", "Short press");
		httpdPushMessage("/push/listen.push", "Short press");

}

LOCAL void ICACHE_FLASH_ATTR user_plug_long_press(void)
{
	os_printf("User press long !!!!!!!!!!!!!!! \r\n");
	httpdPushMessage("/push/io.push", "Long press");
		httpdPushMessage("/push/listen.push", "Long press");
}


void initIntGpio() {
	single_key[0] = key_init_single(PLUG_KEY_0_IO_NUM, PLUG_KEY_0_IO_MUX, PLUG_KEY_0_IO_FUNC,
                                    user_plug_long_press, user_plug_short_press);

single_key[1] = key_init_single(PLUG_KEY_2_IO_NUM, PLUG_KEY_2_IO_MUX, PLUG_KEY_2_IO_FUNC,
                                    user_plug_long_press, user_plug_short_press);
    keys.key_num = PLUG_KEY_NUM;
    keys.single_key = single_key;

    key_init(&keys);
}

//Template code for the WLAN page.
void ICACHE_FLASH_ATTR tplPushIndex(HttpdConnData *connData, char *token, void **arg) {
	char buff[1024];
   
   // ETS_GPIO_INTR_ATTACH (func, arg) 
   // ETS_GPIO_INTR_ATTACH (func, arg) 
initIntGpio();
	if (token==NULL) return;
	int i = GPIO_INPUT_GET(BIT0);
	int b = GPIO_INPUT_GET(BIT1);
	os_strcpy(buff, "Unknown");
	if (os_strcmp(token, "GPIO0")==0) {
			if(i == 1) {
				os_strcpy(buff, "1");
			} else {
				os_strcpy(buff, "2");
			}
	}
	if (os_strcmp(token, "GPIO1")==0) {
			if(b == 1) {
				os_strcpy(buff, "1");
			} else {
				os_strcpy(buff, "2");
			}
	}
	espconn_sent(connData->conn, (uint8 *)buff, os_strlen(buff));
}
