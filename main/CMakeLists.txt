idf_component_register(SRCS "http_controller.c" "nvs_util.c" "main.c" "wand_server_task.c" "mpu6050.cpp" "udp_trans.c" "web_server.c" "web_client.c" "tftest.cc" "./service/wifi_service.c" "./service/device_service.c"
    INCLUDE_DIRS "." "./service"
    EMBED_FILES "../html/error.html"
                "../html/favicon.ico"
                "../html/main.js"
                "../html/root.html"
                "../html/main.css")
