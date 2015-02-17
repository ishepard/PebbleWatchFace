#include <pebble.h>

#define KEY_TEMPERATURE 0
#define KEY_CONDITIONS 1
#define KEY_STREET 2

static Window *s_main_window;
static TextLayer *s_time_layer;
static TextLayer *s_date_layer;
static TextLayer *s_weather_layer;
static TextLayer *s_street_layer;
static BitmapLayer *s_background_layer;
static GBitmap *s_background_bitmap;

static GFont s_time_font;
static GFont s_date_font;
static GFont s_weather_font;
static GFont s_street_font;

static void update_time() {
  // Get a tm structure
  time_t temp = time(NULL); 
  struct tm *tick_time = localtime(&temp);

  // Create a long-lived buffer
  static char buffer[] = "00:00";
  static char buffer_date[] = "00 aaa";

    // Write the current hours and minutes into the buffer
  if(clock_is_24h_style() == true) {
    //Use 2h hour format
    strftime(buffer, sizeof("00:00"), "%H:%M", tick_time);
  } else {
    //Use 12 hour format
    strftime(buffer, sizeof("00:00"), "%I:%M", tick_time);
  }
  
  strftime(buffer_date,sizeof("00 aaa"), "%d %b", tick_time);

  // Display this time on the TextLayer
  text_layer_set_text(s_time_layer, buffer);
  text_layer_set_text(s_date_layer, buffer_date);
}

static void main_window_load(Window *window) {
  // Create GFont
  s_time_font = fonts_load_custom_font(resource_get_handle(RESOURCE_ID_FONT_CLOCK_65));
  s_date_font = fonts_load_custom_font(resource_get_handle(RESOURCE_ID_FONT_CLOCK_ITALIC_20));
  s_street_font = fonts_load_custom_font(resource_get_handle(RESOURCE_ID_FONT_ARIAL_20));
  s_weather_font = fonts_load_custom_font(resource_get_handle(RESOURCE_ID_FONT_CLOCK_30));

  // Create time TextLayer
  s_time_layer = text_layer_create(GRect(0, 15, 144, 70));
  text_layer_set_background_color(s_time_layer, GColorClear);
  text_layer_set_text_color(s_time_layer, GColorWhite);
  text_layer_set_text(s_time_layer, "00:00");
  
  // Create date TextLayer
  s_date_layer = text_layer_create(GRect(0, 5, 144, 20));
  text_layer_set_background_color(s_date_layer, GColorClear);
  text_layer_set_text_color(s_date_layer, GColorWhite);
  text_layer_set_text(s_date_layer, "00 aaa");
  
  // Create weather TextLayer
  s_weather_layer = text_layer_create(GRect(75, 100, 69, 50));
  text_layer_set_background_color(s_weather_layer, GColorClear);
  text_layer_set_text_color(s_weather_layer, GColorWhite);
  text_layer_set_text(s_weather_layer, "");
  
  // Create date TextLayer
  s_street_layer = text_layer_create(GRect(0, 150, 144, 20));
  text_layer_set_background_color(s_street_layer, GColorClear);
  text_layer_set_text_color(s_street_layer, GColorWhite);
  text_layer_set_text(s_street_layer, "Yahoo Weather");
  
  // Create GBitmap, then set to created BitmapLayer
  s_background_bitmap = gbitmap_create_with_resource(RESOURCE_ID_IMAGE_ERROR);
  s_background_layer = bitmap_layer_create(GRect(10, 85, 60, 60));
  bitmap_layer_set_bitmap(s_background_layer, s_background_bitmap);

  // Apply to TextLayer
  text_layer_set_font(s_time_layer, s_time_font);
  text_layer_set_font(s_date_layer, s_date_font);
  text_layer_set_font(s_weather_layer, s_weather_font);

  text_layer_set_text_alignment(s_time_layer, GTextAlignmentCenter);
  text_layer_set_text_alignment(s_date_layer, GTextAlignmentCenter);
  text_layer_set_text_alignment(s_weather_layer, GTextAlignmentCenter);
  text_layer_set_text_alignment(s_street_layer, GTextAlignmentCenter);

  // Add it as a child layer to the Window's root layer
  layer_add_child(window_get_root_layer(window), text_layer_get_layer(s_time_layer));
  layer_add_child(window_get_root_layer(window), text_layer_get_layer(s_date_layer));
  layer_add_child(window_get_root_layer(window), text_layer_get_layer(s_weather_layer));
  layer_add_child(window_get_root_layer(window), text_layer_get_layer(s_street_layer));
  layer_add_child(window_get_root_layer(window), bitmap_layer_get_layer(s_background_layer));

  // Make sure the time is displayed from the start
  update_time();
}

static void main_window_unload(Window *window) {
  // Destroy TextLayer
  text_layer_destroy(s_time_layer);
  text_layer_destroy(s_date_layer);
  text_layer_destroy(s_weather_layer);
  text_layer_destroy(s_street_layer);
  bitmap_layer_destroy(s_background_layer);
  
  // Destroy GBitmap
  gbitmap_destroy(s_background_bitmap);

  // Unload GFont
  fonts_unload_custom_font(s_time_font);
  fonts_unload_custom_font(s_date_font);
  fonts_unload_custom_font(s_weather_font);
  fonts_unload_custom_font(s_street_font);
}

static void tick_handler(struct tm *tick_time, TimeUnits units_changed) {
  update_time();
  
  // Get weather update every 30 minutes
  if(tick_time->tm_min % 30 == 0) {
    // Begin dictionary
    DictionaryIterator *iter;
    app_message_outbox_begin(&iter);

    // Add a key-value pair
    dict_write_uint8(iter, 0, 0);

    // Send the message!
    app_message_outbox_send();
  }
}

static void set_bitmap_from_condition_code(int condition_code){
    if (condition_code == 30){
      s_background_bitmap = gbitmap_create_with_resource(RESOURCE_ID_IMAGE_PARTLY_CLOUDY_DAY);
      bitmap_layer_set_bitmap(s_background_layer, s_background_bitmap);
  } else if (condition_code == 29){
      s_background_bitmap = gbitmap_create_with_resource(RESOURCE_ID_IMAGE_PARTLY_CLOUDY_NIGHT);
      bitmap_layer_set_bitmap(s_background_layer, s_background_bitmap);
  } else if ((condition_code >= 26 && condition_code <= 28) || condition_code == 44) {
      s_background_bitmap = gbitmap_create_with_resource(RESOURCE_ID_IMAGE_CLOUDY);
      bitmap_layer_set_bitmap(s_background_layer, s_background_bitmap);
  } else if (condition_code == 31 || condition_code == 33) {
      s_background_bitmap = gbitmap_create_with_resource(RESOURCE_ID_IMAGE_CLEAR_NIGHT);
      bitmap_layer_set_bitmap(s_background_layer, s_background_bitmap);
  } else if (condition_code == 32 || condition_code == 34 || condition_code == 36) {
      s_background_bitmap = gbitmap_create_with_resource(RESOURCE_ID_IMAGE_CLEAR_DAY);
      bitmap_layer_set_bitmap(s_background_layer, s_background_bitmap);
  } else if (
        (condition_code >= 41 && condition_code <= 43) || condition_code == 16 ||
        condition_code == 13 || condition_code == 15 || condition_code == 25 || 
        condition_code == 46
      ){
      s_background_bitmap = gbitmap_create_with_resource(RESOURCE_ID_IMAGE_SNOW);
      bitmap_layer_set_bitmap(s_background_layer, s_background_bitmap);
  } else if (condition_code >= 19 && condition_code <= 22){
      s_background_bitmap = gbitmap_create_with_resource(RESOURCE_ID_IMAGE_FOG);
      bitmap_layer_set_bitmap(s_background_layer, s_background_bitmap);
  } else if ( 
        condition_code == 3 || condition_code == 4 || condition_code == 9 ||
        condition_code == 11 || condition_code == 12 || condition_code == 40 ||
        condition_code == 45 || condition_code == 47
      ){
      s_background_bitmap = gbitmap_create_with_resource(RESOURCE_ID_IMAGE_RAIN);
      bitmap_layer_set_bitmap(s_background_layer, s_background_bitmap);
  } else if (
        condition_code == 23 || condition_code == 24 || (condition_code >= 0 && condition_code <= 2) ||
        (condition_code >= 37 && condition_code <= 39)
      ){
      s_background_bitmap = gbitmap_create_with_resource(RESOURCE_ID_IMAGE_WIND);
      bitmap_layer_set_bitmap(s_background_layer, s_background_bitmap);
  } else if ( 
        (condition_code >= 5 && condition_code <= 8) || condition_code == 10 || 
        condition_code >= 14 || condition_code <= 17 || condition_code == 18 || 
        condition_code == 35
      ){
      s_background_bitmap = gbitmap_create_with_resource(RESOURCE_ID_IMAGE_SLEET);
      bitmap_layer_set_bitmap(s_background_layer, s_background_bitmap);
  } else if (condition_code == 3200){
    s_background_bitmap = gbitmap_create_with_resource(RESOURCE_ID_IMAGE_ERROR);
    bitmap_layer_set_bitmap(s_background_layer, s_background_bitmap);
  }
}

static void inbox_received_callback(DictionaryIterator *iterator, void *context) {
  // Store incoming information
  static char temperature_buffer[8];
  static char conditions_buffer[8];
  static char weather_layer_buffer[32];
  static char street_buffer[32];
  int condition_code = 3200;
  
  // Read first item
  Tuple *t = dict_read_first(iterator);

  // For all items
  while(t != NULL) {
    // Which key was received?
    switch(t->key) {
    case KEY_TEMPERATURE:
      snprintf(temperature_buffer, sizeof(temperature_buffer), "%d C", (int)t->value->int32);
      break;
    case KEY_CONDITIONS:
      condition_code = (int)t->value->int32;
      snprintf(conditions_buffer, sizeof(conditions_buffer), "%d", (int)t->value->int32);
      break;
    case KEY_STREET:
      snprintf(street_buffer, sizeof(street_buffer), "%s", t->value->cstring);
      break;
    default:
      APP_LOG(APP_LOG_LEVEL_ERROR, "Key %d not recognized!", (int)t->key);
      break;
    }

    // Look for next item
    t = dict_read_next(iterator);
  }

  set_bitmap_from_condition_code(condition_code);
  
  // Assemble full string and display
  snprintf(weather_layer_buffer, sizeof(weather_layer_buffer), "%s", temperature_buffer);
  text_layer_set_text(s_weather_layer, weather_layer_buffer);
  text_layer_set_text(s_street_layer, street_buffer);
  
}

static void inbox_dropped_callback(AppMessageResult reason, void *context) {
  APP_LOG(APP_LOG_LEVEL_ERROR, "Message dropped!");
}

static void outbox_failed_callback(DictionaryIterator *iterator, AppMessageResult reason, void *context) {
  APP_LOG(APP_LOG_LEVEL_ERROR, "Outbox send failed!");
}

static void outbox_sent_callback(DictionaryIterator *iterator, void *context) {
  APP_LOG(APP_LOG_LEVEL_INFO, "Outbox send success!");
}

static void init() {
  // Create main Window element and assign to pointer
  s_main_window = window_create();

  // Set handlers to manage the elements inside the Window
  window_set_window_handlers(s_main_window, (WindowHandlers) {
    .load = main_window_load,
    .unload = main_window_unload
  });
  
  window_set_fullscreen(s_main_window, true);
  window_set_background_color(s_main_window, GColorBlack);
  // Show the Window on the watch, with animated=true
  window_stack_push(s_main_window, true);
  
  // Register with TickTimerService
  tick_timer_service_subscribe(MINUTE_UNIT, tick_handler);
  
  // Register callbacks
  app_message_register_inbox_received(inbox_received_callback);
  app_message_register_inbox_dropped(inbox_dropped_callback);
  app_message_register_outbox_failed(outbox_failed_callback);
  app_message_register_outbox_sent(outbox_sent_callback);
  
  // Open AppMessage
  app_message_open(app_message_inbox_size_maximum(), app_message_outbox_size_maximum());
}

static void deinit() {
  // Destroy Window
  window_destroy(s_main_window);
}

int main(void) {
  init();
  app_event_loop();
  deinit();
}

