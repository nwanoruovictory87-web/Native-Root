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
void close_browser(void){
   fprintf(stdout, "Browser closed! \n");
   gtk_main_quit();
};
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
void on_js_message(WebKitUserContentManager *manager, WebKitJavascriptResult *js_result, gpointer user_data) {
    JSCValue *value = webkit_javascript_result_get_js_value(js_result);
    char *str = jsc_value_to_string(value);
    printf("JS clicked! Data from JS: %s\n", str); // This is your C backend
    g_free(str);
};
//create webview and innitailize global varable
void create_webview_and_manage(Web_View *Wbv){
   Wbv->manager= webkit_user_content_manager_new();// this is like settings for the blank canvas created eg webview
   Wbv->webview= WEBKIT_WEB_VIEW(webkit_web_view_new_with_user_content_manager(Wbv->manager));// this creates the webview content like a blank canvas
   // 3. Register bridge: JS can call window.webkit.messageHandlers.myBackend.postMessage()
    webkit_user_content_manager_register_script_message_handler(Wbv->manager, "myBackend");
    g_signal_connect(Wbv->manager, "script-message-received::myBackend", G_CALLBACK(on_js_message), NULL);
};
// refresh webview
void refresh_wbeview(Web_View *Wbv){
   webkit_web_view_reload(Wbv->webview);
};
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
//
int main(int argc, char *argv[]){
   Gtk_Browser GtkB;// create global browser gtk data
   Web_View Wbv;
   create_browser_window(argc, argv, &GtkB);
   create_webview_and_manage(&Wbv);
   webkit_web_view_load_uri(Wbv.webview, "file:///home/victory/c-asm-home-space/Native-Root/static/index.html");
   enable_developer_settings(&Wbv);
   activate_browser_window(&GtkB, &Wbv);//note anything after this is not runned as code stop here
   //fprintf(stdout, "log %d and %s \n", argc, argv[0]);
   
   return 0;
};
