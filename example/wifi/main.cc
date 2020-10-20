/*  WiFi softAP Example

   This example code is in the Public Domain (or CC0 licensed, at your option.)

   Unless required by applicable law or agreed to in writing, this
   software is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR
   CONDITIONS OF ANY KIND, either express or implied.
*/

#include "bzd.h"
#include "esp_event.h"
#include "esp_log.h"
#include "esp_system.h"
#include "esp_wifi.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "lwip/err.h"
#include "lwip/sockets.h"
#include "lwip/sys.h"
#include "nvs_flash.h"

#include <lwip/netdb.h>
#include <string.h>
#include <sys/param.h>

/* The examples use WiFi configuration that you can set via project configuration menu.

   If you'd rather not, just change the below entries to strings with
   the config you want - ie #define EXAMPLE_WIFI_SSID "mywifissid"
*/
#define EXAMPLE_ESP_WIFI_SSID "ESP32_BLAISE"
#define EXAMPLE_ESP_WIFI_PASS ""
#define EXAMPLE_MAX_STA_CONN 2

#define CONFIG_EXAMPLE_IPV4 1
#define PORT 3333

static const char* TAG = "wifi softAP";

static void wifi_event_handler(void* /*arg*/, esp_event_base_t /*event_base*/, int32_t event_id, void* event_data)
{
	if (event_id == WIFI_EVENT_AP_STACONNECTED)
	{
		wifi_event_ap_staconnected_t* event = (wifi_event_ap_staconnected_t*)event_data;
		ESP_LOGI(TAG, "station " MACSTR " join, AID=%d", MAC2STR(event->mac), event->aid);
	}
	else if (event_id == WIFI_EVENT_AP_STADISCONNECTED)
	{
		wifi_event_ap_stadisconnected_t* event = (wifi_event_ap_stadisconnected_t*)event_data;
		ESP_LOGI(TAG, "station " MACSTR " leave, AID=%d", MAC2STR(event->mac), event->aid);
	}
}

void wifi_init_softap()
{
	tcpip_adapter_init();
	ESP_ERROR_CHECK(esp_event_loop_create_default());

	wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
	ESP_ERROR_CHECK(esp_wifi_init(&cfg));

	ESP_ERROR_CHECK(esp_event_handler_register(WIFI_EVENT, ESP_EVENT_ANY_ID, &wifi_event_handler, NULL));

	wifi_config_t wifi_config{};
	strcpy(reinterpret_cast<char*>(wifi_config.ap.ssid), EXAMPLE_ESP_WIFI_SSID);
	wifi_config.ap.ssid_len = strlen(EXAMPLE_ESP_WIFI_SSID);
	strcpy(reinterpret_cast<char*>(wifi_config.ap.password), EXAMPLE_ESP_WIFI_PASS);
	wifi_config.ap.max_connection = EXAMPLE_MAX_STA_CONN;
	wifi_config.ap.authmode = WIFI_AUTH_WPA_WPA2_PSK;

	if (strlen(EXAMPLE_ESP_WIFI_PASS) == 0)
	{
		wifi_config.ap.authmode = WIFI_AUTH_OPEN;
	}

	ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_AP));
	ESP_ERROR_CHECK(esp_wifi_set_config(ESP_IF_WIFI_AP, &wifi_config));
	ESP_ERROR_CHECK(esp_wifi_start());

	ESP_LOGI(TAG, "wifi_init_softap finished. SSID:%s password:%s", EXAMPLE_ESP_WIFI_SSID, EXAMPLE_ESP_WIFI_PASS);
}

static void tcp_server_task(void* /*pvParameters*/)
{
	char addr_str[128];
	int addr_family;
	int ip_protocol;

#ifdef CONFIG_EXAMPLE_IPV4
	struct sockaddr_in dest_addr;
	dest_addr.sin_addr.s_addr = htonl(INADDR_ANY);
	dest_addr.sin_family = AF_INET;
	dest_addr.sin_port = htons(PORT);
	addr_family = AF_INET;
	ip_protocol = IPPROTO_IP;
	inet_ntoa_r(dest_addr.sin_addr, addr_str, sizeof(addr_str) - 1);
#else // IPV6
	struct sockaddr_in6 dest_addr;
	// bzero(&dest_addr.sin6_addr.un, sizeof(dest_addr.sin6_addr.un));
	dest_addr.sin6_family = AF_INET6;
	dest_addr.sin6_port = htons(PORT);
	addr_family = AF_INET6;
	ip_protocol = IPPROTO_IPV6;
	inet6_ntoa_r(dest_addr.sin6_addr, addr_str, sizeof(addr_str) - 1);
#endif

	int listen_sock = socket(addr_family, SOCK_STREAM, ip_protocol);
	if (listen_sock < 0)
	{
		ESP_LOGE(TAG, "Unable to create socket: errno %d", errno);
		vTaskDelete(NULL);
		return;
	}
	ESP_LOGI(TAG, "Socket created");

	int err = bind(listen_sock, (struct sockaddr*)&dest_addr, sizeof(dest_addr));
	if (err != 0)
	{
		ESP_LOGE(TAG, "Socket unable to bind: errno %d", errno);
		goto CLEAN_UP;
	}
	ESP_LOGI(TAG, "Socket bound, port %d", PORT);

	err = listen(listen_sock, 1);
	if (err != 0)
	{
		ESP_LOGE(TAG, "Error occurred during listen: errno %d", errno);
		goto CLEAN_UP;
	}

	while (1)
	{
		ESP_LOGI(TAG, "Socket listening");

		struct sockaddr_in6 source_addr; // Large enough for both IPv4 or IPv6
		size_t addr_len = sizeof(source_addr);
		int sock = accept(listen_sock, (struct sockaddr*)&source_addr, &addr_len);
		if (sock < 0)
		{
			ESP_LOGE(TAG, "Unable to accept connection: errno %d", errno);
			break;
		}

		// Convert ip address to string
		if (source_addr.sin6_family == PF_INET)
		{
			inet_ntoa_r(((struct sockaddr_in*)&source_addr)->sin_addr.s_addr, addr_str, sizeof(addr_str) - 1);
		}
		else if (source_addr.sin6_family == PF_INET6)
		{
			inet6_ntoa_r(source_addr.sin6_addr, addr_str, sizeof(addr_str) - 1);
		}
		bzd::format::toString(bzd::getOut(), CSTR("Socket accepted ip address: {}"), addr_str);

		ESP_LOGI(TAG, "Socket accepted ip address: %s", addr_str);

		// do_retransmit(sock);

		shutdown(sock, 0);
		close(sock);
	}

CLEAN_UP:
	close(listen_sock);
	vTaskDelete(NULL);
}

int main()
{
	ESP_LOGI(TAG, "START");

	bzd::format::toString(bzd::getOut(), CSTR("START.\n"));

	// Initialize NVS
	esp_err_t ret = nvs_flash_init();
	if (ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND)
	{
		ESP_LOGI(TAG, "nvs_flash_init in if");

		ESP_ERROR_CHECK(nvs_flash_erase());
		ret = nvs_flash_init();
	}
	ESP_LOGI(TAG, "nvs_flash_init past");

	ESP_ERROR_CHECK(ret);

	ESP_LOGI(TAG, "ESP_WIFI_MODE_AP");
	wifi_init_softap();

	bzd::format::toString(bzd::getOut(), CSTR("start task.\n"));

	xTaskCreate(tcp_server_task, "tcp_server", 4096, NULL, 5, NULL);

	return 0;
}
