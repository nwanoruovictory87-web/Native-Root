#include <sys/socket.h>//import system socket files and Apis
#include <netinet/in.h> //has a struct proparty to discripe how  theIp and port would be
#include <unistd.h> //this contains function read();, write(), and close();
#include <stdlib.h> //helper functions
#include <string.h>// helper string functions
#include <pthread.h>// multitread
#include <stdio.h>
#include <gtk-3.0/gtk/gtk.h>
#include <webkitgtk-4.1/webkit2/webkit2.h>
//
typedef struct Root_browser
{
   GtkWidget *window;
   
}Gtk_Browser;
typedef struct Web_View
{
  WebKitWebView *webview;
  WebKitUserContentManager *manager;
}Web_View;
typedef struct SOCKET{
   int server_fd;
   int port;
}SOCKET_DATA;
//
void close_browser(void){
   fprintf(stdout, "Browser closed! \n");
   gtk_main_quit();
};
void procces_client_requst(int *client_id){
   char clientReqBuffer[4086];
   read(*(client_id), clientReqBuffer, sizeof(clientReqBuffer));
   fprintf(stdout, "clinet requst %s \n", (clientReqBuffer));
   char reqProtocal[10]={'\0'};
   char path[100]={'\0'};
   int reqProtocalLength=0;
   while(clientReqBuffer[reqProtocalLength] != ' '){//get protocal
      reqProtocal[reqProtocalLength]= clientReqBuffer[reqProtocalLength];
      reqProtocalLength++;
   };
   //get script path
   //note add 1 to reqProtocalLength as  reqProtocalLength stoped at ' ' added 1 moves it to the next index which is /;
   reqProtocalLength += 1;
   int pathStartCount=0;
   while (clientReqBuffer[reqProtocalLength] != ' ')
   {
      path[pathStartCount]= clientReqBuffer[reqProtocalLength];
      reqProtocalLength++;
      pathStartCount++;
   };
   fprintf(stdout, "data %s rp length %d and path is %s\n", reqProtocal, reqProtocalLength, path);
   if(!(reqProtocal[0] == 'G' && reqProtocal[1] == 'E' && reqProtocal[2] =='T')){// validate that protocal is GET
      char *header400="HTTP/1.1 400 Bad Requst\r\n\r\n";
      write(*(client_id), header400, strlen(header400));
      close(*(client_id));
      return;
   };
   char *base="./static";
   char fullFilePath[100]={'\0'};
   //add base
   int baseCount=0;
   while (base[baseCount] != '\0')
   {
      fullFilePath[baseCount]= base[baseCount];
       baseCount++;
   };
   //add requst path
   int reqPathCount=0;
   while (path[reqPathCount] != '\0')
   {
      fullFilePath[baseCount]= path[reqPathCount];
      baseCount++;
      reqPathCount++;
   }
   char *basePathScript="index.html";//note why as i didnt add / before index is that / is already added to fullFilePath as pathStartCount at 1 = /
   if(pathStartCount == 1){//if pathStartCount is max 1 add basePathScript to fullPath
      int basePathScriptCount=0;
      while(basePathScript[basePathScriptCount] != '\0'){
         fullFilePath[baseCount]= basePathScript[basePathScriptCount];
         baseCount++;
         basePathScriptCount++;
      };
   };
   fprintf(stdout, "base count length %d full path %s and pathCount %d\n", baseCount, fullFilePath, pathStartCount);
   FILE *indexHtml = fopen(fullFilePath, "rb");
   if(!indexHtml){// 404 res
      fclose(indexHtml);
      char *res404= "HTTP/1.1 404 Not Found\r\n\r\n Not Found";
      write(*(client_id), res404, strlen(res404));
      close(*(client_id));
   }else{
      fseek(indexHtml, 0, SEEK_END);// go to end of file
      long fileLength= ftell(indexHtml);// tell position = file size in bytes
      fseek(indexHtml, 0, SEEK_SET);// go back to start
      char *fileContentBuffer= malloc(fileLength + 1);// alocate some memory to save file connet to as to not have overflow on stack if file is to big
      fread(fileContentBuffer, 1, fileLength, indexHtml);// read the file and write to buffer
      fileContentBuffer[fileLength]= '\0'; // add null tarminator to indicate end of file
      fclose(indexHtml);
      char header[256];
      snprintf(header, sizeof(header), "HTTP/1.1 200 OK\r\nContent-Type: text/html\r\nContent-Length: %ld\r\nConnection: close\r\n\r\n", fileLength);
      write(*(client_id), header, strlen(header));
      write(*(client_id), fileContentBuffer, fileLength);
      free(fileContentBuffer);
      close(*(client_id));
   }
   
} 
void* listen_on_client_connection(void *Socket ){
   SOCKET_DATA *p_socket= (SOCKET_DATA *)(Socket);
   while (1)
   {
      fprintf(stdout, "Listing on new requst \n");
      int client_id= accept((p_socket->server_fd), NULL, NULL);
      if(client_id == -1) continue;
      procces_client_requst(&client_id);
   };
}
void create_http_socket(SOCKET_DATA *Socket){
   //create http socket that returns a port
   //note socket returns 1 if it failed to create socket else 3 if succesfull
    (Socket->server_fd)= socket(AF_INET, SOCK_STREAM, 0); //AF_INET is discriping the kind of IP that the file should have in this case IPV4 and this SOCK_STREAM tells it we need a tcp socket if it was SOCK_DRAM it means UDP, and last agument means the keynal get to pick the protocall for tcp
    if((Socket->server_fd) == -1){
      fprintf(stdout, "Filed to create socket exiting \n");
      exit(1);
   }else{
      fprintf(stdout, "TCP socket file created \n");
   };
   //create IP and Port address
   struct sockaddr_in address_port; //this struct defines the ip and port of IPV4;
   memset(&address_port, 0, sizeof(address_port));
   address_port.sin_family = AF_INET; // define the what kind of port is created in this caseIPV4 family
   address_port.sin_addr.s_addr=INADDR_ANY; //inet_addr("127.0.0.1");// define what ip address should the socket listen on INADDR_ANY makes it listin on all ip on the device while inet_addr("port") picks a particulay port to listen on
   address_port.sin_port= htons(8080);// this is like seting a domain on the ip so client can connect on port 8080 which returns ip   127.0.0.1 to connect to why use htons function becourse the cpu sets it as binery htons converts it to decimal to big india
   //time to initailes the socket and bind it to an ID to keep track
   if(bind((Socket->server_fd), (void *)(struct sockaddr_in *)(&address_port), sizeof(address_port))== -1){// what happens here is that am saying take this socket i have created which is address_port and attech it to my socket file id which is socket_fd and heres the size of the address
     (Socket->server_fd)= 0;// set back to zero as it filed to listen
      fprintf(stdout, "Erro socket not runing \n");
      exit(1);
   }; 
   /*the above code 
   step 1 requst a blank socket file with aguments type of socket whould the file be which returns a number like and address/id. 
   step 2 define the socket structure ist IP type IP listining address and its domain/port that connects to the ip
   step 3 bind the socket structure to the blank socket file id in step one which makes socket_fd inherite all the related structure of step two 
   note but this not yet visible i only define how the socket whould be and get and address for the socket but  its not yet runing
   step 3 is to tell the os to listin on this socket by pasing the id then the os checks the socket type and port to listin on  */
   int isSocketListining= listen((Socket->server_fd), 10);/*this listen on the socket the next aguments says how many requst can be pending/qured before rejecting the next requst*/ //it returns 0 on succes 1 on err
   if(!isSocketListining) {
      (Socket->port)=(address_port.sin_port);
      fprintf(stdout, "Socket runing on port :8080 and ip = %d \n", (Socket->port));
    }else{
      (Socket->server_fd)= 0;// set back to zero as it filed to listen
      fprintf(stdout, "Erro socket not runing \n");
      exit(1);
    };
}
//create gtk window/body frame
void create_browser_window(int argc, char *argv[], Gtk_Browser *GtkB){
   fprintf(stdout, "Root Browser Started! \n");
   gtk_init(&argc, &argv); //this starts gtk fun flow
   (GtkB->window)= gtk_window_new(GTK_WINDOW_TOPLEVEL); //initailize global browser
   gtk_window_set_default_size(GTK_WINDOW(GtkB->window), 800, 500);// set default window size
   gtk_window_set_title(GTK_WINDOW(GtkB->window), "Native Root");
   g_signal_connect(GtkB->window, "destroy", G_CALLBACK(close_browser), NULL);// add listiner on window close button clicked
};
//activate window
void activate_browser_window(Gtk_Browser *GtkB, Web_View *Wbv){
   gtk_container_add(GTK_CONTAINER(GtkB->window), GTK_WIDGET(Wbv->webview));
   gtk_widget_show_all(GtkB->window);
   gtk_main();
};
// C function that JS will call
void listen_on_frontend_message(WebKitUserContentManager *manager, WebKitJavascriptResult *js_result, gpointer user_data) {
    JSCValue *value = webkit_javascript_result_get_js_value(js_result);
    char *str = jsc_value_to_string(value);
    printf("JS clicked! Data from JS: %s\n", str); // This is your C backend
    g_free(str);
};
void push_to_frontend(Web_View *Wbv, char data[]){
   webkit_web_view_evaluate_javascript(Wbv->webview, "BRIGE_LISTEN('Hi from c')", -1, NULL, NULL, NULL, NULL, NULL);
}
//create webview and innitailize global varable
void create_webview_manage_and_link_to_backend(Web_View *Wbv){
   Wbv->manager= webkit_user_content_manager_new();// this is like settings for the blank canvas created eg webview
   Wbv->webview= WEBKIT_WEB_VIEW(webkit_web_view_new_with_user_content_manager(Wbv->manager));// this creates the webview content like a blank canvas
   // 3. Register bridge: JS can call window.webkit.messageHandlers.myBackend.postMessage()
    webkit_user_content_manager_register_script_message_handler(Wbv->manager, "myBackend");
    g_signal_connect(Wbv->manager, "script-message-received::myBackend", G_CALLBACK(listen_on_frontend_message), NULL);
};
// refresh webview
void refresh_wbeview(Web_View *Wbv){
   webkit_web_view_reload(Wbv->webview);
};
//
// get html title
void * get_html_title(Web_View *Wbv){
const char *title= webkit_web_view_get_title(Wbv->webview);
void *allTitleMem= malloc(sizeof(title));
(allTitleMem) = (void *)(title);
 return allTitleMem;
};
// enable developer settings
void enable_developer_settings(Web_View *Wbv){
WebKitSettings *settings = webkit_web_view_get_settings(Wbv->webview);
webkit_settings_set_enable_javascript(settings, TRUE);
webkit_settings_set_enable_developer_extras(settings, TRUE); // right-click inspect
}
//load fronend
void load_fronted(Web_View *Wbv, char path[], int isPath, char *url, SOCKET_DATA *Socket, pthread_t *Socket_thread){
   if(isPath){//if isPath load url added by client
   webkit_web_view_load_uri(Wbv->webview, (url));
   }else{// load default vanila js script in static folder
   create_http_socket(Socket);
   pthread_create(Socket_thread, NULL, listen_on_client_connection, Socket);
   g_usleep(1000 *1000);// wait 1 second before moving to the next line
   fprintf(stdout, "Loading file from static folder \n");
   webkit_web_view_load_uri(Wbv->webview, path);
   };
};
int main(int argc, char *argv[]){
   int isPathAdded=0;	
   int count=0;
   char *argvAtIndex2= argv[1];		
   if((argvAtIndex2) != NULL){
     isPathAdded= 1;
   };	
   Gtk_Browser GtkB;// create global browser gtk data
   Web_View Wbv;
   SOCKET_DATA Socket; Socket.server_fd= 0;// set file id to 0 so as on older function call i can know if file was created or note
   pthread_t Socket_thread;
   create_browser_window(argc, argv, &GtkB);
   create_webview_manage_and_link_to_backend(&Wbv);
   enable_developer_settings(&Wbv);
   load_fronted(&Wbv, "http://localhost:8080/", isPathAdded, argvAtIndex2, &Socket, &Socket_thread);
   push_to_frontend(&Wbv, "hhh");
   activate_browser_window(&GtkB, &Wbv);//note anything after this is not runned as code stop here
   //fprintf(stdout, "log %d and %s \n", argc, argv[0]);
   
   return 0;
};
