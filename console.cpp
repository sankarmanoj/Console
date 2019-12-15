#include<iostream>
#include <X11/Xlib.h> // Every Xlib program must include this
#include <assert.h>   // I include this to test return values the lazy way
#include <unistd.h>
#include <map>
#include <list>
typedef std::list<std::string> display_buffer;
std::map<int,Window> AllWindows;
std::map<int,display_buffer> AllDisplayBuffers;
std::map<int,GC> AllGCs;
Display *dpy;
const int window_height = 300;
const int window_width = 300;
const int padding = 50;
const int line_height = 13;
const int max_num_lines = 4;
int place_window_height = 0;
int place_window_width = 0;

void display_from_buffer(int window_index)
{
  Window this_window = AllWindows[window_index];
  display_buffer this_buffer = AllDisplayBuffers[window_index];
  GC this_gc = AllGCs[window_index];
  int start_height = 10;
  XClearWindow(dpy,this_window);
  for(const std::string &this_string : this_buffer)
  {
    XDrawString(dpy,this_window,this_gc,10,start_height,this_string.c_str(),this_string.length());
    start_height += line_height;
  }
  XFlush(dpy);
}
int main()
{
  dpy = XOpenDisplay(0);
  if(dpy==0)
  {
    std::cerr<<"Error in Opening Display"<<std::endl;
    return -1;
  }

  int blackColor = BlackPixel(dpy, DefaultScreen(dpy));
  int whiteColor = WhitePixel(dpy, DefaultScreen(dpy));
  Screen * this_screen = XDefaultScreenOfDisplay(dpy);

  int screen_width = XWidthOfScreen(this_screen);
  int screen_height = XHeightOfScreen(this_screen);
  printf("Screen Dimensions = %dx%d\n",screen_width,screen_height);
  int window_index;
  std::string buffer;
  int focus_revert;
  Window current_focus_window;
  XGetInputFocus(dpy,&current_focus_window,&focus_revert);

  bool done = false;
  while (!done)
  {
    if(std::getline(std::cin,buffer))
    {
      int token_position = buffer.find(":~");
      if(token_position==std::string::npos)
      {
        std::cerr<<"Invalid Input String"<<std::endl;
        continue;
      }
      try
      {
        window_index = std::stoi(buffer.substr(0,token_position));
      }
      catch(...)
      {
        std::cerr<<"Invalid Input String"<<std::endl;
        continue;
      }
      if(AllWindows.find(window_index)==AllWindows.end()) //Index not found in windows map
      {
        Window this_window=XCreateSimpleWindow(dpy, DefaultRootWindow(dpy), place_window_width, place_window_height, window_width, window_height, 0, blackColor, blackColor);
        AllWindows[window_index]=this_window;
        AllDisplayBuffers[window_index] = * (new display_buffer());
        AllGCs[window_index] = XCreateGC(dpy, this_window, 0, 0);

        XSetForeground(dpy, AllGCs[window_index], whiteColor);
        XSetBackground(dpy,AllGCs[window_index],blackColor);

        std::string window_name = "Display "+std::to_string(window_index);
        XStoreName(dpy,this_window,window_name.c_str());

        XSelectInput(dpy, this_window, ExposureMask|KeyPressMask);
        XMapWindow(dpy, this_window);

        XFlush(dpy);

      }
      AllDisplayBuffers[window_index].push_back(buffer.substr(token_position+2));
      if(AllDisplayBuffers[window_index].size()>max_num_lines)
      {
        AllDisplayBuffers[window_index].pop_front();
      }
      display_from_buffer(window_index);

      // std::cout<<"Message is "<<buffer.substr(token_position+2)<<" index "<<window_index<<std::endl;
    }

    XEvent e;

    while(XPending(dpy))
    {
      XNextEvent(dpy, &e);
      /* draw or redraw the window */
      if (e.type == Expose)
      {
        for(auto const& x : AllDisplayBuffers)
        {
          display_from_buffer(x.first);
        }
      }

      if(e.type==KeyPress)
      {

      }

      // Handle Windows Close Event
      if(e.type==ClientMessage)
      {
        break;
      }
    }
  }
  XCloseDisplay(dpy);
  return 0;
}
