#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <string.h>

#define SIZE 1000 
  
// Takes string to be encoded as input 
// and its length and returns encoded string 
char* base64Encoder(char input_str[], int len_str) 
{ 
    // Character set of base64 encoding scheme 
    char char_set[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/"; 
      
    // Resultant string 
    char *res_str = (char *) malloc(SIZE * sizeof(char)); 
      
    int index, no_of_bits = 0, padding = 0, val = 0, count = 0, temp; 
    int i, j, k = 0; 
      
    // Loop takes 3 characters at a time from  
    // input_str and stores it in val 
    for (i = 0; i < len_str; i += 3) 
        { 
            val = 0, count = 0, no_of_bits = 0; 
  
            for (j = i; j < len_str && j <= i + 2; j++) 
            { 
                // binary data of input_str is stored in val 
                val = val << 8;  
                  
                // (A + 0 = A) stores character in val 
                val = val | input_str[j];  
                  
                // calculates how many time loop  
                // ran if "MEN" -> 3 otherwise "ON" -> 2 
                count++; 
              
            } 
  
            no_of_bits = count * 8;  
  
            // calculates how many "=" to append after res_str. 
            padding = no_of_bits % 3;  
  
            // extracts all bits from val (6 at a time)  
            // and find the value of each block 
            while (no_of_bits != 0)  
            { 
                // retrieve the value of each block 
                if (no_of_bits >= 6) 
                { 
                    temp = no_of_bits - 6; 
                      
                    // binary of 63 is (111111) f 
                    index = (val >> temp) & 63;  
                    no_of_bits -= 6;          
                } 
                else
                { 
                    temp = 6 - no_of_bits; 
                      
                    // append zeros to right if bits are less than 6 
                    index = (val << temp) & 63;  
                    no_of_bits = 0; 
                } 
                res_str[k++] = char_set[index]; 
            } 
    } 
  
    // padding is done here 
    for (i = 1; i <= padding; i++)  
    { 
        res_str[k++] = '='; 
    } 
  
    res_str[k] = '\0;'; 
  
    return res_str; 
  
} 
  

int main(int argc, char *argv[])
{
	char *proxy_address, *my_url, *proxy_port, *username, *password, *filename, *logoname;
	my_url = argv[1];
	proxy_address = argv[2];
	proxy_port = argv[3];
	username = argv[4];
	password = argv[5];
	filename = argv[6];
	logoname = argv[7];
	int port = atoi(proxy_port);

	//create socket
	int client_socket;
	client_socket = socket(AF_INET, SOCK_STREAM, 0);

	struct sockaddr_in remote_address;
	remote_address.sin_family = AF_INET;
	remote_address.sin_port = htons(port);
	remote_address.sin_addr.s_addr = inet_addr(proxy_address);
	//inet_aton(proxy_address, &remote_address.sin_addr.s_addr);

	//connect
	connect(client_socket, (struct sockaddr *)&remote_address, sizeof(remote_address));

	//deal with request message
	//char request[] = "GET http://jandarshan.cg.nic.in/ HTTP/1.1\r\nProxy-Authorization: Basic Y3NmMzAzOmNzZjMwMw==\r\n\r\n";
	char request[200] = "GET ";
	char end_with[] = "\r\n";
	char space[] = " ";
	char http_prot[] = "HTTP/1.1";
	char proxy_code[] = "Proxy-Authorization: Basic ";

	//base64 encoding
	int len_input = strlen(username) + 1 + strlen(password);
	char input_str[len_input+1];
	strcat(input_str, username);
	strcat(input_str, ":");
	strcat(input_str, password); 
    int len_str; 
  	
  	printf("%s", input_str);
    // calculates length of string 
    len_str = sizeof(input_str) / sizeof(input_str[0]); 
      
    // to exclude '\0' character 
    len_str -= 1;  
  
    char *encoded64 = base64Encoder(input_str, len_str);
    int code_len = strlen(encoded64);
    encoded64[code_len-1] = '\0';
    printf("%s", encoded64);

	strcat(request, my_url);
	strcat(request, space);
	strcat(request, http_prot);
	strcat(request, end_with);
	strcat(request, proxy_code);
	strcat(request, encoded64);
	strcat(request, end_with);
	strcat(request, end_with);

	//printf("%s", request);

	char response[30000];

	send(client_socket, request, sizeof(request), 0);
	
	
	FILE *fp;
	
	fp = fopen("index.html", "w+");
	
	char buf[1024];
	int bytes_read;
	do
	{
		bytes_read = recv(client_socket, buf, sizeof(buf), 0);

		fputs(buf, fp);
	}while(bytes_read > 0);

	//printf("%s",response);
	fclose(fp);
	close(client_socket);
	return 0;
}