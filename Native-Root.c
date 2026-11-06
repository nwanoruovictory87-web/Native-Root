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
void create_browser_window(int argc, char *argv[], Gtk_Browser *GtkB){
   fprintf(stdout, "Root Browser Started! \n");
   gtk_init(&argc, &argv); //this starts gtk fun flow
   (GtkB->window)= gtk_window_new(GTK_WINDOW_TOPLEVEL); //initailize global browser
   gtk_window_set_default_size(GTK_WINDOW(GtkB->window), 1200, 800);// set default window size
   gtk_window_set_title(GTK_WINDOW(GtkB->window), "Root Browser");
   g_signal_connect(GtkB->window, "destroy", G_CALLBACK(close_browser), NULL);// add listiner on window close button clicked
};
void activate_browser_window(Gtk_Browser *GtkB, Web_View *Wbv){
   gtk_container_add(GTK_CONTAINER(GtkB->window), GTK_WIDGET(Wbv->webview));
   gtk_widget_show_all(GtkB->window);
   gtk_main();
};

void create_webview_and_manage(Web_View *Wbv){
   Wbv->webview= WEBKIT_WEB_VIEW(webkit_web_view_new());// this creates the webview content like a blank canvas
  Wbv->manager= webkit_user_content_manager_new();// this is like settings for the blank canvas created eg webview
}

int main(int argc, char *argv[]){
   Gtk_Browser GtkB;// create global browser gtk data
   Web_View Wbv;
   create_browser_window(argc, argv, &GtkB);
   create_webview_and_manage(&Wbv);
   webkit_web_view_load_html(Wbv.webview, "<h1 style{{color: \"white\"}}>Hello world</h1>", NULL);
   activate_browser_window(&GtkB, &Wbv);
   //fprintf(stdout, "log %d and %s \n", argc, argv[0]);
   
   return 0;
};
