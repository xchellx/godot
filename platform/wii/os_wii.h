#ifndef OS_WII_H
#define OS_WII_H

#include "drivers/unix/os_unix.h"
#include "servers/visual/visual_server_raster.h"
#include "servers/visual/visual_server_wrap_mt.h"
#include "main/input_default.h"
#include "audio_driver_wii.h"

class OS_Wii : public OS {

    int video_driver_index;
    MainLoop *main_loop;
    VisualServer *visual_server;
    InputDefault *input;
    VideoMode video_mode;
    AudioDriverWii wii_audio_driver;

    bool force_quit;

protected:
    virtual void initialize_core();
    virtual Error initialize(const VideoMode &p_desired, int p_video_driver, int p_audio_driver);

    virtual void set_main_loop(MainLoop *p_main_loop);
    virtual void delete_main_loop();

    virtual void finalize();
    virtual void finalize_core();

    virtual bool _check_internal_feature_support(const String &p_feature);

public:
    static FILE *log_file;

    virtual void alert(const String &p_alert, const String &p_title = "ALERT!") { printf((const char *)p_alert.c_str());}
    virtual String get_stdin_string(bool p_block = true) {return "";}

    virtual Point2 get_mouse_position() const {return Point2();}
    virtual int get_mouse_button_state() const {return 0;}
    virtual void set_window_title(const String &p_title) {}

    virtual void set_video_mode(const VideoMode &p_video_mode, int p_screen = 0);
    virtual VideoMode get_video_mode(int p_screen = 0) const {return video_mode;}
    virtual void get_fullscreen_mode_list(List<VideoMode> *p_list, int p_screen = 0) const;

    virtual int get_video_driver_count() const {return 1;}
    virtual const char *get_video_driver_name(int p_driver) const {return "GX";}
    virtual int get_current_video_driver() const {return video_driver_index;}

    virtual int get_audio_driver_count() const {return 1;}
    virtual const char *get_audio_driver_name(int p_driver) const {return "Dummy";}

    virtual Size2 get_window_size() const {return Size2(640, 480);}

    virtual bool is_window_always_on_top() const {return true;}
    virtual Error execute(const String &p_path, const List<String> &p_arguments, bool p_blocking = true, ProcessID *r_child_id = NULL, String *r_pipe = NULL, int *r_exitcode = NULL, bool read_stderr = false, Mutex *p_pipe_mutex = NULL);
    virtual Error kill(const ProcessID &p_pid);

    // TODO: Can Wii do execute/shell stuff? Probably not but check later

    virtual bool has_environment(const String &p_var) const {return false;}
    virtual String get_environment(const String &p_var) const {return "";}
    virtual bool set_environment(const String &p_var, const String &p_value) const {return false;}

    virtual String get_name() const {return "Wii";}
    
    virtual MainLoop *get_main_loop() const {return main_loop;}

    virtual Date get_date(bool utc) const;
    virtual Time get_time(bool utc) const;
    virtual TimeZoneInfo get_time_zone_info() const;

    virtual void delay_usec(uint32_t p_usec) const;
    virtual uint64_t get_ticks_usec() const;

    virtual bool can_draw() const;

    // TODO: Virtual keyboard?

    void run();

    OS_Wii();
    ~OS_Wii();
};

#endif