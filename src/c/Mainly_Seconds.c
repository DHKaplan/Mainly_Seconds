#include "pebble.h"

Window      *window;

TextLayer   *text_time_layer;
TextLayer   *text_battery_layer;

Layer       *window_layer;

Layer       *BTLayer;

GFont        fontHelvNewLight20;
GFont		     fontRobotoCondensed21;
GFont        fontRobotoBoldSubset49;

GRect        bluetooth_rect;

static int  batterychargepct;
static int  BatteryVibesDone = 0;
static int  batterycharging = 0;

GPoint     Linepoint;
static int BTConnected = 1;
static int BTVibesDone = 0;
 
static int  PersistBGColor        = 0;
static int  PersistTextColor      = 0;
static int  PersistBTLoss         = 0;     // 0 = No Vib, 1 = Vib
static int  PersistLow_Batt       = 0;     // 0 = No Vib, 1 = Vib

static int FirstTime = 0;


static char time_text[] = "00:00";
static char seconds_text[] = "00";

GColor GTextColorHold;
GColor GBGColorHold;
GColor ColorHold;

void handle_bluetooth(bool connected) {
     if (connected) {
         BTConnected = 1;     // Connected
         BTVibesDone = 0;

    } else {
         BTConnected = 0;      // Not Connected

         if ((BTVibesDone == 0) && (PersistBTLoss == 1)) {    
             BTVibesDone = 1;
             vibes_long_pulse();
         }
    }
    layer_mark_dirty(BTLayer);
}

//BT Logo Callback;
void BTLine_update_callback(Layer *BTLayer, GContext* BT1ctx) {

       GPoint BTLinePointStart;
       GPoint BTLinePointEnd;

       graphics_context_set_stroke_color(BT1ctx, GTextColorHold);
       graphics_context_set_fill_color(BT1ctx, GBGColorHold); 
        
  
    if (BTConnected == 0) {
            graphics_context_set_fill_color(BT1ctx, GColorWhite);
      
        #ifdef PBL_COLOR
            graphics_context_set_stroke_color(BT1ctx, GColorRed);
            graphics_fill_rect(BT1ctx, layer_get_bounds(BTLayer), 0, GCornerNone);
        #else
           graphics_context_set_stroke_color(BT1ctx, GColorBlack);
           graphics_fill_rect(BT1ctx, layer_get_bounds(BTLayer), 0, GCornerNone);
        #endif 

        // "X"" Line 1
        BTLinePointStart.x = 1;
        BTLinePointStart.y = 1;

        BTLinePointEnd.x = 20;
        BTLinePointEnd.y = 20;
        graphics_draw_line(BT1ctx, BTLinePointStart, BTLinePointEnd);

        // "X"" Line 2
        BTLinePointStart.x = 1;
        BTLinePointStart.y = 20;

        BTLinePointEnd.x = 20;
        BTLinePointEnd.y = 1;
        graphics_draw_line(BT1ctx, BTLinePointStart, BTLinePointEnd);
      }
      else
      {
       //Line 1
       BTLinePointStart.x = 10;
       BTLinePointStart.y = 1;

       BTLinePointEnd.x = 10;
       BTLinePointEnd.y = 20;
       graphics_draw_line(BT1ctx, BTLinePointStart, BTLinePointEnd);

       //Line 2
       BTLinePointStart.x = 10;
       BTLinePointStart.y = 1;

       BTLinePointEnd.x = 16;
       BTLinePointEnd.y = 5;
       graphics_draw_line(BT1ctx, BTLinePointStart, BTLinePointEnd);

       //Line 3
       BTLinePointStart.x = 4;
       BTLinePointStart.y = 5;

       BTLinePointEnd.x = 16;
       BTLinePointEnd.y = 16;
       graphics_draw_line(BT1ctx, BTLinePointStart, BTLinePointEnd);

       //Line 4
       BTLinePointStart.x = 4;
       BTLinePointStart.y = 16;

       BTLinePointEnd.x = 16;
       BTLinePointEnd.y = 5;
       graphics_draw_line(BT1ctx, BTLinePointStart, BTLinePointEnd);

       //Line 5
       BTLinePointStart.x = 10;
       BTLinePointStart.y = 20;

       BTLinePointEnd.x = 16;
       BTLinePointEnd.y = 16;
       graphics_draw_line(BT1ctx, BTLinePointStart, BTLinePointEnd);
      }
}

void handle_battery(BatteryChargeState charge_state) {
  static char BatteryPctTxt[] = "+100%";

  batterychargepct = charge_state.charge_percent;

  if (charge_state.is_charging) {
    batterycharging = 1;
    APP_LOG(APP_LOG_LEVEL_INFO, "Battery Charging");
  } else {
    batterycharging = 0;
    APP_LOG(APP_LOG_LEVEL_INFO, "Battery *NOT* Charging");
  }

  // Reset if Battery > 20% ********************************
  if ((batterychargepct > 20) && (batterycharging == 0)) {
      BatteryVibesDone = 0;

      text_layer_set_background_color(text_battery_layer, GBGColorHold);
      text_layer_set_text_color(text_battery_layer, GTextColorHold);       
  }
  
  if ((batterychargepct < 30) && (batterycharging == 0)) {
     if (BatteryVibesDone == 0) {            // Do Once
         BatteryVibesDone = 1;
         #ifdef PBL_COLOR
           text_layer_set_text_color(text_battery_layer, GColorRed);
         #else
           text_layer_set_text_color(text_battery_layer, GColorBlack);
         #endif
       
         text_layer_set_background_color(text_battery_layer, GColorWhite);
       
         APP_LOG(APP_LOG_LEVEL_WARNING, "Battery Vibes Sent");
         vibes_long_pulse();
      }
  }

   if (charge_state.is_charging) {
     strcpy(BatteryPctTxt, "Chrg");
  } else {
     snprintf(BatteryPctTxt, 5, "%d%%", charge_state.charge_percent);
  }
   text_layer_set_text(text_battery_layer, BatteryPctTxt);
   
}


void handle_appfocus(bool in_focus){
    if (in_focus) {
        handle_bluetooth(bluetooth_connection_service_peek());
        handle_battery(battery_state_service_peek());
    }
}

//       ******************** Main Loop **************************************************************

void handle_tick(struct tm *tick_time, TimeUnits units_changed) {
  char time_format[] = "%I:%M";

  strftime(seconds_text, sizeof(seconds_text), "%S", tick_time);
   
  if (clock_is_24h_style()) {
       strcpy(time_format,"%R");
     } else {
       strcpy(time_format,"%I:%M");
     }  
  
  strftime(time_text, sizeof(time_text), time_format, tick_time);

  // Kludge to handle lack of non-padded hour format string
  // for twelve hour clock.
  if (!clock_is_24h_style() && (time_text[0] == '0')) {
       memmove(time_text, &time_text[1], sizeof(time_text) - 1);
  }
    

     //Always set time  *****************************************************
  static char timeit[]="00:00:00";
  strftime(timeit, sizeof(timeit), "%I:%M:%S", tick_time);
      
    
     text_layer_set_text(text_time_layer, time_text); 
  
  FirstTime = 1; 
}

//Receive Input from Config html page: * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * 

static void inbox_received_callback(DictionaryIterator *iterator, void *context) {
  APP_LOG(APP_LOG_LEVEL_ERROR, "In Inbox received callback * * * * * * *");
    
  FirstTime = 0;
  
  #ifdef PBL_COLOR
          Tuple *BG_Color = dict_find(iterator, MESSAGE_KEY_BG_COLOR_KEY);     
            
         if (BG_Color) { // Config Exists
             PersistBGColor = BG_Color->value->int32;
             APP_LOG(APP_LOG_LEVEL_INFO,      "    Added Config Local BG Color: %d", PersistBGColor);   
         } else { //Check for Persist
               if(persist_exists(MESSAGE_KEY_BG_COLOR_KEY)) {
                  PersistBGColor = persist_read_int(MESSAGE_KEY_BG_COLOR_KEY);
                  APP_LOG(APP_LOG_LEVEL_INFO, "    Added Persistant Local BG Color = %d", PersistBGColor);
               }  else {   // Set Default
                  PersistBGColor = 255; 
                  APP_LOG(APP_LOG_LEVEL_INFO, "    Added Default Local BG Color %d", PersistBGColor);
               }
            }
         
         ColorHold = GColorFromHEX(PersistBGColor);
         persist_write_int(MESSAGE_KEY_BG_COLOR_KEY,   PersistBGColor);
        
       #else
         ColorHold = GColorBlack;
       #endif
  
        GBGColorHold = ColorHold;
   
        text_layer_set_background_color(text_battery_layer, ColorHold);
        text_layer_set_background_color(text_time_layer, ColorHold);
        window_set_background_color(window, ColorHold);
  
  //*****
  
    #ifdef PBL_COLOR
        Tuple *Text_Color =  dict_find(iterator, MESSAGE_KEY_TEXT_COLOR_KEY);
 
         if (Text_Color) { // Config Exists
             PersistTextColor = Text_Color->value->int32;
             ColorHold = GColorFromHEX(PersistTextColor);
             persist_write_int(MESSAGE_KEY_TEXT_COLOR_KEY, Text_Color->value->int32);
        
             APP_LOG(APP_LOG_LEVEL_INFO,    "    Added Config Local Text Color: %d", PersistTextColor);   
         } else { //Check for Persist
               if(persist_exists(MESSAGE_KEY_TEXT_COLOR_KEY)) {
                  PersistTextColor = persist_read_int(MESSAGE_KEY_TEXT_COLOR_KEY);
                  ColorHold = GColorFromHEX(PersistTextColor);
                  APP_LOG(APP_LOG_LEVEL_INFO, "    Added Persistant Local Text Color = %d", PersistTextColor);
               }  else {   // Set Default
                  PersistTextColor = 16777215; 
                    ColorHold = GColorFromHEX(PersistTextColor);

                  APP_LOG(APP_LOG_LEVEL_INFO, "    Added Default Local Text %d", PersistTextColor);
               }
            }
         
         persist_write_int(MESSAGE_KEY_TEXT_COLOR_KEY, PersistTextColor);
       #else
         ColorHold = GColorWhite;
       #endif
  
        GTextColorHold = ColorHold;
     
        text_layer_set_text_color(text_battery_layer, ColorHold);
        text_layer_set_text_color(text_time_layer, ColorHold);
  
  // For all items * * * * *
      
     //Vibrate on BT Loss
     Tuple *BTVib = dict_find(iterator, MESSAGE_KEY_BT_VIBRATE_KEY);  
      
      if(BTVib) {
        PersistBTLoss = BTVib->value->int32;
          APP_LOG(APP_LOG_LEVEL_INFO,      "    Added Config Vib on BT Loss: %d", PersistBTLoss);   
      } else { //Check for Persist
            if(persist_exists(MESSAGE_KEY_BT_VIBRATE_KEY)) {
               PersistBTLoss = persist_read_int(MESSAGE_KEY_BT_VIBRATE_KEY);
               APP_LOG(APP_LOG_LEVEL_INFO, "    Added Persistant Vib on BT Loss: %d", PersistBTLoss);
            }  else {   // Set Default
               PersistBTLoss = 0;  // Default NO Vibrate
               APP_LOG(APP_LOG_LEVEL_INFO, "    Added Default Vib on BT Loss: %d", PersistBTLoss);
             }
      }
      
     
     //Vibrate on Low Batt
     Tuple *LowBatt = dict_find(iterator, MESSAGE_KEY_LOW_BATTERY_KEY);  
      
     if(LowBatt) {
        PersistLow_Batt = LowBatt->value->int32;
        APP_LOG(APP_LOG_LEVEL_INFO,      "    Added Config Vib on Low Batt: %d", PersistLow_Batt);   
      } else { //Check for Persist
            if(persist_exists(MESSAGE_KEY_LOW_BATTERY_KEY)) {
               PersistLow_Batt = persist_read_int(MESSAGE_KEY_LOW_BATTERY_KEY);
               APP_LOG(APP_LOG_LEVEL_INFO, "    Added Persistant Vib on Low Batt: %d", PersistLow_Batt);
            }  else {   // Set Default
               PersistLow_Batt = 0;  // Default NO Vibrate
               APP_LOG(APP_LOG_LEVEL_INFO, "    Added Default Vib on Low Batt: %d", PersistLow_Batt);
             }
      } 
      
  FirstTime = 0;
}

static void inbox_dropped_callback(AppMessageResult reason, void *context) {
  APP_LOG(APP_LOG_LEVEL_ERROR, "Inbox Message dropped!");
}

static void outbox_failed_callback(DictionaryIterator *iterator, AppMessageResult reason, void *context) {
  APP_LOG(APP_LOG_LEVEL_ERROR, "Outbox send failed!");
}

static void outbox_sent_callback(DictionaryIterator *iterator, void *context) {
  APP_LOG(APP_LOG_LEVEL_INFO, "Outbox send success!");
}

void handle_deinit(void) {
  APP_LOG(APP_LOG_LEVEL_ERROR, "Writing Persistant BTVib %d", PersistBTLoss);
  
  persist_write_int(MESSAGE_KEY_BT_VIBRATE_KEY,           PersistBTLoss);
  APP_LOG(APP_LOG_LEVEL_ERROR, "Writing Persistant Low Batt Vib %d", PersistLow_Batt);

  persist_write_int(MESSAGE_KEY_LOW_BATTERY_KEY,          PersistLow_Batt);
  
  persist_write_int(MESSAGE_KEY_BG_COLOR_KEY,             PersistBGColor);
  
  persist_write_int(MESSAGE_KEY_TEXT_COLOR_KEY,           PersistTextColor);

  tick_timer_service_unsubscribe();
  battery_state_service_unsubscribe();
  bluetooth_connection_service_unsubscribe();
  app_focus_service_unsubscribe();

  text_layer_destroy(text_time_layer);

  layer_destroy(BTLayer);

  fonts_unload_custom_font(fontHelvNewLight20);
 
  window_destroy(window);
}

//********************************** Handle Init **************************
void handle_init(void) {
  APP_LOG(APP_LOG_LEVEL_ERROR, "In Init... * * * * * * *");
  
  //Set Default Colors
  if(persist_exists(MESSAGE_KEY_BG_COLOR_KEY)) {
     PersistBGColor = persist_read_int(MESSAGE_KEY_BG_COLOR_KEY); 
     GBGColorHold = GColorFromHEX(PersistBGColor);
     APP_LOG(APP_LOG_LEVEL_INFO, "    Set BGColor to Persistant %d", PersistBGColor);    
  }  else {
     #ifdef PBL_COLOR
        APP_LOG(APP_LOG_LEVEL_INFO, "    Set PBL_COLOR BGColor to Default GColorDukeBlue");
        GBGColorHold = GColorDukeBlue;
     #else
        APP_LOG(APP_LOG_LEVEL_INFO, "    Set Non PBL_COLOR BGColor to Default GColorBlack");
        GBGColorHold = GColorBlack;
     #endif    
  } 
  #ifdef PBL_COLOR
  if(persist_exists(MESSAGE_KEY_TEXT_COLOR_KEY)) {
     PersistTextColor = persist_read_int(MESSAGE_KEY_TEXT_COLOR_KEY);  
     GTextColorHold = GColorFromHEX(PersistTextColor);
     APP_LOG(APP_LOG_LEVEL_INFO, "    Set TextColor to Persistant %d", PersistTextColor);    
  }  else {
     APP_LOG(APP_LOG_LEVEL_INFO, "    Set PBL_Color TextColor to Default White");        
     GTextColorHold = GColorWhite;  
  } 
  #else
     APP_LOG(APP_LOG_LEVEL_INFO, "    Set Non PBL_COLOR TextColor to Default White");        
     GTextColorHold = GColorWhite;  
  #endif
  
  window = window_create();

  window_set_background_color(window, GBGColorHold);

  window_stack_push(window, true /* Animated */);
  
  fontHelvNewLight20 = fonts_load_custom_font(resource_get_handle(RESOURCE_ID_FONT_HELV_NEW_LIGHT_20));
 
  window_layer = window_get_root_layer(window);

  // Register callbacks
  app_message_register_inbox_received(inbox_received_callback);
  app_message_register_inbox_dropped(inbox_dropped_callback);
  app_message_register_outbox_failed(outbox_failed_callback);
  app_message_register_outbox_sent(outbox_sent_callback);

  // Open AppMessage
  app_message_open(128, 128);
     
  //Persistent Value Vib On BTLoss
  if(persist_exists(MESSAGE_KEY_BT_VIBRATE_KEY)) {
     PersistBTLoss = persist_read_int(MESSAGE_KEY_BT_VIBRATE_KEY);  
     APP_LOG(APP_LOG_LEVEL_INFO, "    Set BT Vibrate To Persistant %d (0 = NO Vib, 1 = Vib", PersistBTLoss);
  }  else {
     PersistBTLoss = 0; // Default
     APP_LOG(APP_LOG_LEVEL_INFO, "    Set BT Vibrate To 0 Default - No Vibrate");

  } 

  //Persistent Value Vib on Low Batt
  if(persist_exists(MESSAGE_KEY_LOW_BATTERY_KEY)) {
     PersistLow_Batt = persist_read_int(MESSAGE_KEY_LOW_BATTERY_KEY);  
     APP_LOG(APP_LOG_LEVEL_INFO, "    Set Low Batt Vibrate To Persistant %d (0 = NO Vib, 1 = Vib", PersistLow_Batt);
  }  else {
     PersistLow_Batt = 0; // Default
     APP_LOG(APP_LOG_LEVEL_INFO, "    Set Low Batt Vibrate To 0 Default - No Vibrate");

  } 
  // Time of Day
  #ifdef PBL_PLATFORM_CHALK
      text_time_layer = text_layer_create(GRect(1, 116, 180, 180-116));
  #else //Aplite or Basalt
      text_time_layer = text_layer_create(GRect(1, 116, 144, 168-116));
  #endif
    
  text_layer_set_text_alignment(text_time_layer, GTextAlignmentCenter);
    #ifdef PBL_PLATFORM_CHALK
        text_layer_set_font(text_time_layer, fonts_get_system_font(FONT_KEY_BITHAM_42_BOLD));
    #else //Aplite or Basalt
        text_layer_set_font(text_time_layer, fonts_get_system_font(FONT_KEY_ROBOTO_BOLD_SUBSET_49));
    #endif  
      
  text_layer_set_text_color(text_time_layer, GTextColorHold);
  text_layer_set_background_color(text_time_layer, GBGColorHold);
  
  layer_add_child(window_layer, text_layer_get_layer(text_time_layer));
  
  
  tick_timer_service_subscribe(SECOND_UNIT, handle_tick);

  //Bluetooth Logo Setup area
  #ifdef PBL_PLATFORM_CHALK
      GRect BTArea = GRect(55, 5, 20, 20);
  #else
      GRect BTArea = GRect(1, 5, 20, 20);
  #endif 
    
  BTLayer = layer_create(BTArea);

  layer_add_child(window_layer, BTLayer);

  layer_set_update_proc(BTLayer, BTLine_update_callback);

  bluetooth_connection_service_subscribe(&handle_bluetooth);
  handle_bluetooth(bluetooth_connection_service_peek());

  // Battery Text Setup
  #ifdef PBL_PLATFORM_CHALK
      text_battery_layer = text_layer_create(GRect(80,2,55,26));
  #else
      text_battery_layer = text_layer_create(GRect(85,2,55,26));
  #endif 
    
  text_layer_set_text_color(text_battery_layer, GTextColorHold);
  text_layer_set_background_color(text_battery_layer, GBGColorHold);
  text_layer_set_font(text_battery_layer, fontHelvNewLight20);
  text_layer_set_text_alignment(text_battery_layer, GTextAlignmentRight);

  battery_state_service_subscribe(&handle_battery);

  handle_battery(battery_state_service_peek());

  layer_add_child(window_layer, text_layer_get_layer(text_battery_layer));

  window_set_background_color(window, GBGColorHold);

    
  //app focus service subscribe
  app_focus_service_subscribe(&handle_appfocus);
  
  // Ensures time is displayed immediately (will break if NULL tick event accessed).
  // (This is why it's a good idea to have a separate routine to do the update itself.)
  time_t now = time(NULL);
  struct tm *current_time = localtime(&now);
  handle_tick(current_time, SECOND_UNIT);
}

int main(void) {
   handle_init();

   app_event_loop();

   handle_deinit();
}
