#include "bili_http.h"
#include "updata.h"

//#define WEB_URL "/x/relation/stat?vmid=138699039"
BILIBILI wp_fans;
static const char *TAG = "HTTP";

struct DATA_WEATHER weather_data[3];

esp_err_t cjson_bilibili(char *text);
esp_err_t cjson_face(char *text);
bool _get_face_url = false;
char url_buff[256] = {0};
char *face_buffer;
static esp_err_t bilibili_http_event_handler(esp_http_client_event_t *evt)
{

	switch (evt->event_id)
	{
	case HTTP_EVENT_ERROR:
		ESP_LOGI(TAG, "HTTP_EVENT_ERROR");
		break;
	case HTTP_EVENT_ON_CONNECTED:
		ESP_LOGI(TAG, "HTTP_EVENT_ON_CONNECTED");
		break;
	case HTTP_EVENT_HEADER_SENT:
		ESP_LOGI(TAG, "HTTP_EVENT_HEADER_SENT");
		break;
	case HTTP_EVENT_ON_HEADER:
		ESP_LOGI(TAG, "HTTP_EVENT_ON_HEADER, key=%s, value=%s", evt->header_key, evt->header_value);
		break;
	case HTTP_EVENT_ON_DATA:
		printf("HTTP_EVENT_ON_DATA, len=%d\n", evt->data_len);

		//如果404或者502
		if (evt->data_len == 150 || evt->data_len == 146)
			return ESP_OK;

		// printf("%.*s\n", evt->data_len, (char *)evt->data);
		cjson_bilibili((char *)evt->data);
		break;
	case HTTP_EVENT_ON_FINISH:
		ESP_LOGI(TAG, "HTTP_EVENT_ON_FINISH");
		break;
	case HTTP_EVENT_DISCONNECTED:
		ESP_LOGI(TAG, "HTTP_EVENT_DISCONNECTED");

		break;
	}
	return ESP_OK;
}
static esp_err_t bilibili_face_http_event_handler(esp_http_client_event_t *evt)
{

	switch (evt->event_id)
	{
	case HTTP_EVENT_ERROR:
		ESP_LOGI(TAG, "HTTP_EVENT_ERROR");
		break;
	case HTTP_EVENT_ON_CONNECTED:
		ESP_LOGI(TAG, "HTTP_EVENT_ON_CONNECTED");
		break;
	case HTTP_EVENT_HEADER_SENT:
		ESP_LOGI(TAG, "HTTP_EVENT_HEADER_SENT");
		break;
	case HTTP_EVENT_ON_HEADER:
		ESP_LOGI(TAG, "HTTP_EVENT_ON_HEADER, key=%s, value=%s", evt->header_key, evt->header_value);
		break;
	case HTTP_EVENT_ON_DATA:
		printf("HTTP_EVENT_ON_DATA, len=%d\n", evt->data_len);

		//如果404或者502
		if (evt->data_len == 150 || evt->data_len == 146)
			return ESP_OK;

		// printf("%.*s\n", evt->data_len, (char *)evt->data);
		cjson_face((char *)evt->data);

		break;
	case HTTP_EVENT_ON_FINISH:
		ESP_LOGI(TAG, "HTTP_EVENT_ON_FINISH");
		break;
	case HTTP_EVENT_DISCONNECTED:
		ESP_LOGI(TAG, "HTTP_EVENT_DISCONNECTED");

		break;
	}
	return ESP_OK;
}


void get_bilibili(char *bili_uid)
{
	printf("%s !Dram: %d bytes\r\n", __func__, heap_caps_get_free_size(MALLOC_CAP_INTERNAL | MALLOC_CAP_8BIT));
	static char post_url[256] = {0};
	memset((char *)post_url, 0, 256);
	static esp_http_client_config_t config = {
		// .url = "https://api.bilibili.com/x/relation/stat?vmid=59041601&jsonp=jsonp",
		.event_handler = bilibili_http_event_handler,
		.buffer_size = 3 * 1024,
		.timeout_ms = 4000,
		.path = "/get",
		.transport_type = HTTP_TRANSPORT_OVER_TCP,
	};
	sprintf(post_url, "https://api.bilibili.com/x/relation/stat?vmid=%s&jsonp=jsonp", bili_uid);
	config.url = post_url;
	esp_http_client_handle_t client = esp_http_client_init(&config);
	esp_http_client_set_header(client, "Accept", "*/*");
	esp_http_client_set_header(client, "Accept-Encoding", "identity");
	esp_http_client_set_header(client, "User-Agent", "PostmanRuntime/7.24.1");
	esp_http_client_set_header(client, "Connection", "keep-alive");
	esp_http_client_set_header(client, "Content-Type", "application/json");

	esp_err_t err = esp_http_client_perform(client);
	if (err == ESP_OK)
	{
		ESP_LOGE(TAG, "HTTP GET Status = %d, content_length = %d",
				 esp_http_client_get_status_code(client),
				 esp_http_client_get_content_length(client));
	}
	else
	{
		ESP_LOGE(TAG, "HTTP GET request failed: %s", esp_err_to_name(err));
	}

	esp_http_client_cleanup(client);
}
void get_face_url(char *bili_uid)
{
	printf("%s !Dram: %d bytes\r\n", __func__, heap_caps_get_free_size(MALLOC_CAP_INTERNAL | MALLOC_CAP_8BIT));
	static char post_url[256] = {0};
	memset((char *)post_url, 0, 256);
	static esp_http_client_config_t config = {
		// .url = "https://api.bilibili.com/x/relation/stat?vmid=59041601&jsonp=jsonp",
		.event_handler = bilibili_face_http_event_handler,
		.buffer_size = 3 * 1024,
		.timeout_ms = 4000,
		.path = "/get",
		.transport_type = HTTP_TRANSPORT_OVER_TCP,
	};
	sprintf(post_url, "https://api.bilibili.com/x/space/acc/info?mid=%s", bili_uid);
	config.url = post_url;
	esp_http_client_handle_t client = esp_http_client_init(&config);
	esp_http_client_set_header(client, "Accept", "*/*");
	esp_http_client_set_header(client, "Accept-Encoding", "identity");
	esp_http_client_set_header(client, "User-Agent", "PostmanRuntime/7.24.1");
	esp_http_client_set_header(client, "Connection", "keep-alive");
	esp_http_client_set_header(client, "Content-Type", "application/json");

	esp_err_t err = esp_http_client_perform(client);
	if (err == ESP_OK)
	{
		ESP_LOGE(TAG, "HTTP GET Status = %d, content_length = %d",
				 esp_http_client_get_status_code(client),
				 esp_http_client_get_content_length(client));
	}
	else
	{
		ESP_LOGE(TAG, "HTTP GET request failed: %s", esp_err_to_name(err));
	}

	esp_http_client_cleanup(client);
}


int64_t http_stream_len = 0;
int data_read_num = 1, total_data_read_nums = 0;
char data_read_buf[1024] = {0};
bool is_recv_complete = 0;
/*

	获取B站头像JPG文件buffer
*/
static esp_err_t face_http_event_handler(esp_http_client_event_t *evt)
{
	static char *output_buffer; // Buffer to store response of http request from event handler
	static int output_len;		// Stores number of bytes read

	switch (evt->event_id)
	{
	case HTTP_EVENT_ERROR:
		ESP_LOGI(TAG, "HTTP_EVENT_ERROR");
		break;
	case HTTP_EVENT_ON_CONNECTED:
		ESP_LOGI(TAG, "HTTP_EVENT_ON_CONNECTED");
		break;
	case HTTP_EVENT_HEADER_SENT:
		ESP_LOGI(TAG, "HTTP_EVENT_HEADER_SENT");
		break;
	case HTTP_EVENT_ON_HEADER:
		ESP_LOGI(TAG, "HTTP_EVENT_ON_HEADER, key=%s, value=%s", evt->header_key, evt->header_value);
		break;
	case HTTP_EVENT_ON_DATA:
		printf("HTTP_EVENT_ON_DATA, len=%d\n", evt->data_len);

		//如果404或者502
		// if (evt->data_len == 150 || evt->data_len == 146)
		// 	return ESP_OK;

		// printf("%.*s\n", evt->data_len, (char *)evt->data);
		// if (!esp_http_client_is_chunked_response(evt->client)) {
		//         // Write out data
		//         printf("%.*s", evt->data_len, (char*)evt->data);
		//     }
		if (!esp_http_client_is_chunked_response(evt->client))
		{
			//ESP_LOGI(TAG, "HTTP_EVENT_ON_DATA  !esp_http_client_is_chunked_response");
			// If user_data buffer is configured, copy the response into the buffer
			if (face_buffer) /*如果已经申请了内存 就直接复制接收到的数据*/
			{

				memcpy(face_buffer + output_len, evt->data, evt->data_len);
			}
			else /*如果没有申请*/
			{
				if (face_buffer == NULL) /*申请一个http长度的内存来保存数据*/
				{
					face_buffer = (char *)malloc(esp_http_client_get_content_length(evt->client));
					output_len = 0;
					if (face_buffer == NULL)
					{
						ESP_LOGE(TAG, "Failed to allocate memory for output buffer");
						return ESP_FAIL;
					}
				}
				memcpy(face_buffer + output_len, evt->data, evt->data_len); //拷贝数据
			}

			output_len += evt->data_len; //记录下载进度
		}

		break;
	case HTTP_EVENT_ON_FINISH:
		ESP_LOGI(TAG, "HTTP_EVENT_ON_FINISH");
		break;
	case HTTP_EVENT_DISCONNECTED:
		ESP_LOGI(TAG, "HTTP_EVENT_DISCONNECTED");

		break;
	}
	return ESP_OK;
}
void get_face_jpg(char *bili_uid)
{
	printf("%s !Dram: %d bytes\r\n", __func__, heap_caps_get_free_size(MALLOC_CAP_INTERNAL | MALLOC_CAP_8BIT));

	// static char post_url[256] = {0};
	// memset((char *)post_url, 0, 256);
	static esp_http_client_config_t config = {
		// .url = "https://whycan.com/files/members/4115/11111.png",
		.event_handler = face_http_event_handler,
		.buffer_size = 3 * 1024,
		.timeout_ms = 4000,
		.path = "/get",
		.transport_type = HTTP_TRANSPORT_OVER_TCP,
	};
	printf("url_buff:%s", url_buff);
	config.url = url_buff;
	esp_http_client_handle_t client = esp_http_client_init(&config);
	esp_http_client_set_header(client, "Accept", "*/*");
	esp_http_client_set_header(client, "Accept-Encoding", "identity");
	esp_http_client_set_header(client, "User-Agent", "PostmanRuntime/7.24.1");
	esp_http_client_set_header(client, "Connection", "keep-alive");
	esp_http_client_set_header(client, "Content-Type", "application/json");
	// esp_err_t err = esp_http_client_open(client, 0);
	esp_err_t err = esp_http_client_perform(client);
	if (err == ESP_OK)
	{
		ESP_LOGE(TAG, "HTTP GET Status = %d, content_length = %d",
				 esp_http_client_get_status_code(client),
				 esp_http_client_get_content_length(client));
	}
	else
	{
		ESP_LOGE(TAG, "HTTP GET request failed: %s", esp_err_to_name(err));
	}
	// if (err == ESP_OK) //请求成功
	// {
	// 	printf("esp_http_client_open  DOWN\n\n\n");

	// 	http_stream_len = esp_http_client_fetch_headers(client); //得到即将下载文件的大小，此处为bin文件的大小
	// 	// face_buffer = heap_caps_malloc((sizeof(uint8_t *))*http_stream_len, MALLOC_CAP_SPIRAM);
	// 	face_buffer = (char *)malloc(sizeof(char) * http_stream_len);
	// 	memset(face_buffer, 0, http_stream_len);
	// 	printf("http_stream_len= %d\n\n\n", (int)http_stream_len);

	// 	if (http_stream_len > 0) //存在数据
	// 	{
	// 		while (data_read_num > 0) //循环读取数据
	// 		{
	// 			data_read_num = esp_http_client_read(client, data_read_buf, 1024); //每次读取1K数据，此处缓存越大通讯的次数就会越少

	// 			if (errno == ENOTCONN || errno == ECONNRESET || errno == ECONNABORTED || data_read_num < 0)
	// 			{
	// 				ESP_LOGI(TAG, "Connection closed, errno = %d", errno);
	// 				break;
	// 			}
	// 			else
	// 			{

	// 				// memcpy(face_buffer + total_data_read_nums, data_read_buf, data_read_num);
	// 				//在此处存储数据到flash,此处调用ESP_LOGI函数会导致他处重入，造成打印不出
	// 				printf("data_read_num= %d\r\n", data_read_num);
	// 				// for (size_t i = total_data_read_nums; i < data_read_num+total_data_read_nums; i++)
	// 				// {
	// 				// 	printf("0x%x ",face_buffer[i]);
	// 				// }
	// 			}

	// 			total_data_read_nums += data_read_num;

	// 			if (data_read_num == 0)
	// 			{
	// 				is_recv_complete = esp_http_client_is_complete_data_received(client);

	// 				if (!is_recv_complete) //未接收完，继续接收
	// 				{
	// 					data_read_num = 1;
	// 					ESP_LOGI(TAG, "未接收完，继续接收");
	// 				}
	// 				else
	// 				{
	// 					//此处应增加一个掉电保存的全局变量，以标记下载存储的bin文件是完整的
	// 					ESP_LOGI(TAG, "接收头像完毕！");
	// 					esp_http_client_close(client);
	// 					printf("esp_http_client_close DOWN \n\n\n");

	// 					printf("total_data_read_nums= %d\n\n\n", total_data_read_nums);
	// 					total_data_read_nums = 0;
	// 				}
	// 			}
	// 		}
	// 	}
	// }
	// else
	// {
	// 	ESP_LOGI(TAG, "HTTP request failed: %s", esp_err_to_name(err));
	// }

	esp_http_client_cleanup(client);
}
esp_err_t cjson_bilibili(char *text)
{
	cJSON *root_ksdiy, *psub_ksdiy;
	char *index = strchr(text, '{');
	printf("bilibili json:%s\n", text);
	if (index == NULL)
	{
		return ESP_FAIL;
	}
	else
	{
		strcpy(text, index);
		root_ksdiy = cJSON_Parse(text);
		if (root_ksdiy != NULL)
		{
			psub_ksdiy = cJSON_GetObjectItem(root_ksdiy, "data");
			if (psub_ksdiy != NULL)
			{
				cJSON *follower = cJSON_GetObjectItem(psub_ksdiy, "follower");
				wp_fans.follower = follower->valueint;
				cJSON *following = cJSON_GetObjectItem(psub_ksdiy, "following");
				wp_fans.following = following->valueint;
				ESP_LOGI(TAG, "获取粉丝成功 follower:%d  following:%d\n", wp_fans.follower, wp_fans.following);
			}
			else
			{
				ESP_LOGI(TAG, "获取粉丝失败，请检查uid");
			}
		}
		else
		{
			return ESP_FAIL;
		}
		cJSON_Delete(root_ksdiy);
	}
	return ESP_OK;
}
esp_err_t cjson_face(char *text)
{
	cJSON *root_ksdiy, *psub_ksdiy;
	char *index = strchr(text, '{');
	printf("face json:%s\n", text);
	if (index == NULL)
	{
		return ESP_FAIL;
	}
	else
	{
		strcpy(text, index);
		root_ksdiy = cJSON_Parse(text);
		if (root_ksdiy != NULL)
		{
			psub_ksdiy = cJSON_GetObjectItem(root_ksdiy, "data");
			if (psub_ksdiy != NULL)
			{
				cJSON *name = cJSON_GetObjectItem(psub_ksdiy, "name");
				wp_fans.name = name->valuestring;
				cJSON *face_url = cJSON_GetObjectItem(psub_ksdiy, "face");
				wp_fans.face_url = face_url->valuestring;
				ESP_LOGI(TAG, "获取头像URL成功 name:%s  face_url:%s\n", wp_fans.name, wp_fans.face_url);
				memset((char *)url_buff, 0, 256);
				sprintf(url_buff, "%s", face_url->valuestring);
				_get_face_url = true;
			}
			else
			{
				_get_face_url = true;
				memset((char *)url_buff, 0, 256);
				// _get_face_url = false;
				sprintf(url_buff, "%s","http://i2.hdslb.com/bfs/face/bce14f5e3af4bca480fc7de227986ba304507078.jpg");
				ESP_LOGI(TAG, "获取粉丝失败，使用默认头像");

			}
		}
		else
		{
			return ESP_FAIL;
		}
		cJSON_Delete(root_ksdiy);
	}
	return ESP_OK;
}
esp_err_t cjson_weather(char *text)
{
	cJSON *root, *psub;
	cJSON *arrayItem;
	printf("weather json:%s\n", text);
	char *index = strchr(text, '{');
	if (index == NULL)
	{
		return ESP_FAIL;
	}
	else
	{
		strcpy(text, index);
		root = cJSON_Parse(text);
		if (root != NULL)
		{
			psub = cJSON_GetObjectItem(root, "results");
			if (psub != NULL)
			{
				arrayItem = cJSON_GetArrayItem(psub, 0);
				cJSON *now = cJSON_GetObjectItem(arrayItem, "daily");
				//ESP_LOGI(HTTP_TAG,"code_day:%s",now->valuestring);
				for (uint8_t i = 0; i < 3; i++)
				{
					cJSON *array2 = cJSON_GetArrayItem(now, i);
					cJSON *code_day = cJSON_GetObjectItem(array2, "code_day");
					//sprintf(weather_data[i].code_day,"%d",atoi(code_day->valuestring));
					weather_data[i].code_day = atoi(code_day->valuestring);
					cJSON *text_day = cJSON_GetObjectItem(array2, "text_day");
					strcpy(weather_data[i].text_day, text_day->valuestring);
					cJSON *high = cJSON_GetObjectItem(array2, "high");
					weather_data[i].high = atoi(high->valuestring);
					cJSON *low = cJSON_GetObjectItem(array2, "low");
					weather_data[i].low = atoi(low->valuestring);
					cJSON *rainfall = cJSON_GetObjectItem(array2, "rainfall");
					weather_data[i].rainfall = atof(rainfall->valuestring);
					cJSON *wind_direction_degree = cJSON_GetObjectItem(array2, "wind_direction_degree");
					weather_data[i].wind_direction_degree = atoi(wind_direction_degree->valuestring);
					cJSON *wind_direction = cJSON_GetObjectItem(array2, "wind_direction");
					strcpy(weather_data[i].wind_direction, wind_direction->valuestring);
					cJSON *wind_speed = cJSON_GetObjectItem(array2, "wind_speed");
					weather_data[i].wind_speed = atof(wind_speed->valuestring);
					cJSON *humidity = cJSON_GetObjectItem(array2, "humidity");
					weather_data[i].humidity = atoi(humidity->valuestring);
				}
			}
			else
			{
				ESP_LOGI("HTTP", "获取天气失败，请检测是否有改接口权力");
			}
		}
		else
		{
			return ESP_FAIL;
		}
		cJSON_Delete(root);
	}
	return ESP_OK;
}

static esp_err_t weather_http_event_handler(esp_http_client_event_t *evt)
{

	switch (evt->event_id)
	{
	case HTTP_EVENT_ERROR:
		ESP_LOGI(TAG, "HTTP_EVENT_ERROR");
		break;
	case HTTP_EVENT_ON_CONNECTED:
		ESP_LOGI(TAG, "HTTP_EVENT_ON_CONNECTED");
		break;
	case HTTP_EVENT_HEADER_SENT:
		ESP_LOGI(TAG, "HTTP_EVENT_HEADER_SENT");
		break;
	case HTTP_EVENT_ON_HEADER:
		ESP_LOGI(TAG, "HTTP_EVENT_ON_HEADER, key=%s, value=%s", evt->header_key, evt->header_value);
		break;
	case HTTP_EVENT_ON_DATA:
		printf("HTTP_EVENT_ON_DATA, len=%d\n", evt->data_len);

		//如果404或者502
		if (evt->data_len == 150 || evt->data_len == 146)
			return ESP_OK;

		// printf("%.*s\n", evt->data_len, (char *)evt->data);
		cjson_weather((char *)evt->data);
		break;
	case HTTP_EVENT_ON_FINISH:
		ESP_LOGI(TAG, "HTTP_EVENT_ON_FINISH");
		break;
	case HTTP_EVENT_DISCONNECTED:
		ESP_LOGI(TAG, "HTTP_EVENT_DISCONNECTED");

		break;
	}
	return ESP_OK;
}
void get_weather(char *city, char *key)
{
	printf("%s !Dram: %d bytes\r\n", __func__, heap_caps_get_free_size(MALLOC_CAP_INTERNAL | MALLOC_CAP_8BIT));
	static char post_url[256] = {0};
	memset((char *)post_url, 0, 256);
	static esp_http_client_config_t config = {
		// .url = "https://api.bilibili.com/x/relation/stat?vmid=59041601&jsonp=jsonp",
		.event_handler = weather_http_event_handler,
		.buffer_size = 3 * 1024,
		.timeout_ms = 4000,
		.path = "/get",
		.transport_type = HTTP_TRANSPORT_OVER_TCP,
	};
	sprintf(post_url, "https://api.seniverse.com/v3/weather/daily.json?key=%s&location=%s&language=zh-Hans&unit=c&start=0&days=5",
			key, city);
	config.url = post_url;
	esp_http_client_handle_t client = esp_http_client_init(&config);
	esp_http_client_set_header(client, "Accept", "*/*");
	esp_http_client_set_header(client, "Accept-Encoding", "identity");
	esp_http_client_set_header(client, "User-Agent", "PostmanRuntime/7.24.1");
	esp_http_client_set_header(client, "Connection", "keep-alive");
	esp_http_client_set_header(client, "Content-Type", "application/json");

	esp_err_t err = esp_http_client_perform(client);
	if (err == ESP_OK)
	{
		ESP_LOGE(TAG, "HTTP GET Status = %d, content_length = %d",
				 esp_http_client_get_status_code(client),
				 esp_http_client_get_content_length(client));
	}
	else
	{
		ESP_LOGE(TAG, "HTTP GET request failed: %s", esp_err_to_name(err));
	}

	esp_http_client_cleanup(client);
}

esp_err_t read_weather()
{
	static char city[30] = {0};
	static char key[30] = {0};
	if (read_nvs("city", city)) //获取哔哩哔哩uid
		ESP_LOGI(TAG, "获取到城市");
	else
	{
		sprintf(city, "chengdu");
		ESP_LOGI(TAG, "没有获取到城市，默认使用 成都");
	}
	if (read_nvs("pass", key)) //获取哔哩哔哩uid
		ESP_LOGI(TAG, "获取到心知密匙");
	else
	{
		save_nvs("pass", "SybN2IXZM2B_vayTP");
		sprintf(key, "SybN2IXZM2B_vayTP");
		ESP_LOGI(TAG, "没有获取心知密匙，使用默认:SybN2IXZM2B_vayTP");
	}
	get_weather(city, key);
	return ESP_OK;
}
esp_err_t read_fans()
{
	static char str[20] = {0};
	static char uid[15] = {0};
	if (read_nvs("uid", uid)) //获取哔哩哔哩uid
	{
		sprintf(str, "%s", uid);
		ESP_LOGI(TAG, "获取到uid");
	}
	else
	{
		sprintf(str, "%s", uid);
		save_nvs("uid", "59041601");
		ESP_LOGI(TAG, "没有uid，使用默认uid:59041601");
	}
	ESP_LOGI(TAG, "%s", str);
	get_bilibili(str);
	get_face_url(str);
	if (_get_face_url == true)
		get_face_jpg(str);
	return ESP_OK;
}
uint16_t get_value_ksdiyfs(uint8_t i)
{
	if (i)
		return wp_fans.follower;
	else
	{
		return wp_fans.following;
	}
}
