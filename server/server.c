/*************************************************************************
	> File Name: server.c
	> Author: 
	> Mail: 
	> Created Time: 2017年06月09日 星期五 18时34分57秒
 ************************************************************************/

#include<stdio.h>
#include<sys/socket.h>
#include<sys/types.h>
#include<arpa/inet.h>
#include<netinet/in.h>
#include<string.h>


void Usage(char *msg)
{
    printf("Usage: %s [_ip] [_port}\n", msg);

}
int startup(char * _ip, int port)
{
    int sock = socket(AF_INET, SOCK_STREAM,0);
    if(sock < 0)
    {
        //print_log
        exit(2);
    }

    struct sockaddr_in local;
    local.sin_family = AF_INET;
    local.sin_port = htons(port);
    local.sin_addr.s_addr = inet_addr(_ip);

    if(bind(sock,(struct sockadd *)&local, sizeof(local)) < 0)
    {
        //print_log
        exit(3);
    }


    if(listen(sock , 5) < 0)
    {
        //print_log
        exit(4);
    }

    return sock;
}
int ftpserve_login(int sock)
{

}
void ftpserve_process(int sock)
{
    int sock_data;
    char cmd[5];
    char arg[MAXSIZE];
    
    send_response(sock, 220);

    if(ftpserve_login(sock) == 1)
        send_response(sock, 230);
    else{
        send_response(sock, 430);
        exit(0);
    }

    while(1)
    {
        int _r = ftpserve_get_cmd(sock, cmd, arg);
        if((_r < 0 )||(_r == 221))
        {
            break;
        }
        if(_r == 200)
        {
            if((sock_data = ftpserve_data_connect(sock)) < 0)
            {
                close(sock);
                exit(1);
            }

            if(strcmp(cmd, "LIST") == 0)
                ftpserve_list(sock_data,sock);
            else if(strcmp(cmd, "RETR") == 0){
                ftpserve_retr(sock_data,sock);
            }

            close(sock_data);
        }
    }
}

int main(int argc,char * argv[])
{


    //命令參數合法检测
    if(argc != 3)
    {
        Usage(argv[0]);
        return 1;
    }

    int listen_sock = startup(argv[1], atoi(argv[2]));

    while(1)
    {
        struct sockaddr_in peer;
        socklen_t len = sizeof(peer);

        int sock = accept(listen_sock, (struct sockaddr*)&peer, &len);
        if(sock < 0)
        {
            //print_log
            continue;
        }

        pid_t pid = fork();
        
        if(pid < 0)
        {
            //print_log
            break;
        }
        else if(pid == 0)
        {
            close(listen_sock);
            ftpserve_process(sock);
            close(sock);
            exit(0);
        }
        close(sock);
        wait(NULL);
    }
    close(listen_sock);

    return 0;
}
