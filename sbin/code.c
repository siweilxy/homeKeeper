    memset(params, 0, sizeof(params));
    params[1].buffer_type = MYSQL_TYPE_STRING;
    params[1].buffer = ipInfo.ip;
    params[1].buffer_length = sizeof(ipInfo.ip);

