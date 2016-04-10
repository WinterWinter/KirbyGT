#include <pebble.h>

#define KEY_TEMPERATURE 1
#define KEY_ICON 2
#define KEY_SCALE 3
#define KEY_TEST 4

Window *window;

static GBitmap *s_bitmap = NULL;
static BitmapLayer *s_bitmap_layer;
static GBitmapSequence *s_sequence = NULL;

static GBitmap *foreground_image;
static BitmapLayer *foreground_layer;

static TextLayer *s_weather_layer;

static BitmapLayer *boss_layer;
static GBitmap *boss_bitmap = NULL;

static bool animate = true;
static bool daytime = NULL;

const int KIRBY_IMAGE_RESOURCE_IDS[] = {
  RESOURCE_ID_KIRBY_BEAM,
  RESOURCE_ID_KIRBY_CUTTER,
  RESOURCE_ID_KIRBY_FIRE,
  RESOURCE_ID_KIRBY_HAMMER,
  RESOURCE_ID_KIRBY_MIKE,
  RESOURCE_ID_KIRBY_SWORD,
};

#define TOTAL_POWERS 2
static GBitmap *powers_images[TOTAL_POWERS];
static BitmapLayer *powers_layers[TOTAL_POWERS];

const int POWERS_IMAGE_RESOURCE_IDS[] = {
  RESOURCE_ID_BEAM,
  RESOURCE_ID_CUTTER,
  RESOURCE_ID_FIRE,
  RESOURCE_ID_HAMMER,
  RESOURCE_ID_MIKE,
  RESOURCE_ID_SWORD,
};

static const uint32_t BOSSES_IMAGE_RESOURCE_IDS[] = {
  RESOURCE_ID_MR_BRIGHT,
  RESOURCE_ID_KRACKO,
  RESOURCE_ID_MR_SHINE
};

static uint8_t battery_level;
static bool battery_plugged;
static Layer *battery_layer;

TextLayer *text_time_layer;
TextLayer *text_date_layer;

time_t auto_hide;

static bool initiate_watchface = true;

int NUM_KIRBY = 6;
int seed_images2;
int start_number_image, random_image;

static void set_container_image(GBitmap **bmp_image, BitmapLayer *bmp_layer, const int resource_id, GPoint origin) {
GBitmap *old_image = *bmp_image;

 	*bmp_image = gbitmap_create_with_resource(resource_id);
 	GRect frame = (GRect) {
   	.origin = origin,
    .size = gbitmap_get_bounds(*bmp_image).size 
};
 	bitmap_layer_set_bitmap(bmp_layer, *bmp_image);
 	layer_set_frame(bitmap_layer_get_layer(bmp_layer), frame);

 	if (old_image != NULL) {
 	gbitmap_destroy(old_image);
  }
}

static void timer_handler(void *context) {
  uint32_t next_delay;

  if(gbitmap_sequence_update_bitmap_next_frame(s_sequence, s_bitmap, &next_delay)) {
    bitmap_layer_set_bitmap(s_bitmap_layer, s_bitmap);
    layer_mark_dirty(bitmap_layer_get_layer(s_bitmap_layer));

    if(animate)
    {
      app_timer_register(next_delay, timer_handler, NULL);
    }
   else {
    gbitmap_sequence_restart(s_sequence);
    }
  }
  
  if(time(NULL) == auto_hide){
  
  layer_set_hidden(text_layer_get_layer(text_date_layer), false);

  layer_set_hidden(text_layer_get_layer(s_weather_layer), true);
  }
  
}

static void stop_animation()
{
  animate = false;
  gbitmap_sequence_restart(s_sequence);
}

static void load_sequence() {
  
  if(s_sequence) {
    gbitmap_sequence_destroy(s_sequence);
    s_sequence = NULL;
  }
  if(s_bitmap) {
    gbitmap_destroy(s_bitmap);
    s_bitmap = NULL;
  }
  
  s_sequence = gbitmap_sequence_create_with_resource(KIRBY_IMAGE_RESOURCE_IDS[random_image]);
  s_bitmap = gbitmap_create_blank(gbitmap_sequence_get_bitmap_size(s_sequence), GBitmapFormat8Bit);
  
  animate = true;
  app_timer_register(10, timer_handler, NULL);
  app_timer_register(5000, stop_animation, NULL);
}

static void load_time_text_layer(Layer *window_layer)
{
  text_time_layer = text_layer_create(GRect(1, 130, 72, 26));
 	text_layer_set_text_alignment(text_time_layer, GTextAlignmentCenter);
 	text_layer_set_text_color(text_time_layer, GColorBlack);
 	text_layer_set_background_color(text_time_layer, GColorClear);
  text_layer_set_font(text_time_layer,fonts_get_system_font(FONT_KEY_LECO_20_BOLD_NUMBERS));
 	layer_add_child(window_layer, text_layer_get_layer(text_time_layer));  
}

static void load_date_text_layer(Layer *window_layer)
{
  text_date_layer = text_layer_create(GRect(72, 130, 72, 26));	
	text_layer_set_text_alignment(text_date_layer, GTextAlignmentCenter);
 	text_layer_set_text_color(text_date_layer, GColorBlack);
 	text_layer_set_background_color(text_date_layer, GColorClear);
  text_layer_set_font(text_date_layer,fonts_get_system_font(FONT_KEY_LECO_20_BOLD_NUMBERS));
 	layer_add_child(window_layer, text_layer_get_layer(text_date_layer)); 
}

static void load_kirby_layer(Layer *window_layer)
{ 
  
  if(random_image == 0){
    s_bitmap_layer = bitmap_layer_create(GRect(2, 53, 89, 89));
  }
  else if(random_image == 1){
    s_bitmap_layer = bitmap_layer_create(GRect(0, 79, 113, 30));
  }
  else if(random_image == 2){
    s_bitmap_layer = bitmap_layer_create(GRect(0, 68, 102, 43));
  }
  else if(random_image == 3){
    s_bitmap_layer = bitmap_layer_create(GRect(0, 67, 55, 41));
  }
  else if(random_image == 4){
    s_bitmap_layer = bitmap_layer_create(GRect(9, 78, 50, 29));
  }
  else if(random_image == 5){
    s_bitmap_layer = bitmap_layer_create(GRect(0, 66, 84, 52));
  }
  
  bitmap_layer_set_compositing_mode(s_bitmap_layer, GCompOpSet);
  layer_add_child(window_layer, bitmap_layer_get_layer(s_bitmap_layer));
  
}

static void load_foreground_layer(Layer *window_layer)
{
  foreground_image = gbitmap_create_with_resource(RESOURCE_ID_FOREGROUND);
  foreground_layer = bitmap_layer_create(layer_get_frame(window_layer));
  bitmap_layer_set_compositing_mode(foreground_layer, GCompOpSet);
  
  bitmap_layer_set_bitmap(foreground_layer, foreground_image);
 	layer_add_child(window_layer, bitmap_layer_get_layer(foreground_layer));
}


void battery_layer_update_callback(Layer *layer, GContext *ctx) {

  	graphics_context_set_compositing_mode(ctx, GCompOpAssign);
    
    GColor8 batteryColor = GColorRed;
  
  	graphics_context_set_stroke_color(ctx, batteryColor);
  	graphics_context_set_fill_color(ctx,  batteryColor);

   	graphics_fill_rect(ctx, GRect(0, 0, (uint8_t)(battery_level)/2, 10), 0, GCornerNone);

  	if (!battery_plugged) {
   
   	}
  	else {	
     
 	  }
}

static void load_battery_layer(Layer *window_layer)
{  
 	BatteryChargeState initial = battery_state_service_peek();  
 	battery_level = initial.charge_percent;
 	battery_plugged = initial.is_plugged;
 	battery_layer = layer_create(GRect(9,13,50,10));
 	layer_set_update_proc(battery_layer, &battery_layer_update_callback);  
  layer_add_child(window_layer, battery_layer);
}

static void load_boss_layer(Layer *window_layer)
  {
  boss_layer = bitmap_layer_create(GRect(64, 35, 80, 72));
  layer_add_child(window_layer, bitmap_layer_get_layer(boss_layer));
  bitmap_layer_set_compositing_mode(boss_layer, GCompOpSet);
  }

static void load_weather_layer(Layer *window_layer)
  {
  s_weather_layer = text_layer_create(GRect(72, 130, 72, 26));
  text_layer_set_background_color(s_weather_layer, GColorClear);
  text_layer_set_text_color(s_weather_layer, GColorBlack);
  text_layer_set_text(s_weather_layer, "...");
  text_layer_set_font(s_weather_layer,fonts_get_system_font(FONT_KEY_LECO_20_BOLD_NUMBERS));
  text_layer_set_text_alignment(s_weather_layer, GTextAlignmentCenter);
  layer_add_child(window_layer, text_layer_get_layer(s_weather_layer));
  layer_set_hidden(text_layer_get_layer(s_weather_layer), true);
  }

  static void main_window_load(Window *window) {
  Layer *window_layer = window_get_root_layer(window);

  load_foreground_layer(window_layer);
  load_boss_layer(window_layer);
  load_weather_layer(window_layer);
  load_battery_layer(window_layer);
  load_time_text_layer(window_layer);
  load_date_text_layer(window_layer);
}

static void main_window_unload(Window *window) {  
  
      gbitmap_destroy(foreground_image);
      bitmap_layer_destroy(foreground_layer);
  
     	for (int i = 0; i < TOTAL_POWERS; i++) {
     	layer_remove_from_parent(bitmap_layer_get_layer(powers_layers[i]));
     	gbitmap_destroy(powers_images[i]);
     	bitmap_layer_destroy(powers_layers[i]);
 	    }
  
      gbitmap_destroy(s_bitmap);
      bitmap_layer_destroy(s_bitmap_layer);      
  
      text_layer_destroy(text_time_layer);
      text_layer_destroy(text_date_layer);
      text_layer_destroy(s_weather_layer);
  
      gbitmap_sequence_destroy(s_sequence);
  
      layer_destroy(battery_layer);
  
      bitmap_layer_destroy(boss_layer);
      gbitmap_destroy(boss_bitmap);
}

static void update_time()
{
  time_t temp = time(NULL); 
  struct tm *tick_time = localtime(&temp);
  
  static char time_text[] = "00:00";
 	static char date_text[] = "00.00";

 	char *date_format;
  
	date_format = "%m.%d";
  
  if(clock_is_24h_style() == true) {
    strftime(time_text, sizeof("00:00"), "%H:%M", tick_time);
  } else {
    strftime(time_text, sizeof("00:00"), "%I:%M", tick_time);
  }
  
 	strftime(date_text, sizeof(date_text), date_format, tick_time);  
  if (time_text[0] == '0') {
   		memmove(time_text, &time_text[1], sizeof(time_text) - 1);
	}  
  
  text_layer_set_text(text_time_layer, time_text);
 	text_layer_set_text(text_date_layer, date_text);
}

static void update_bg_color(struct tm *current_time) {
  if (current_time->tm_hour >= 12 && current_time->tm_hour < 17){
  window_set_background_color(window, GColorFromRGB(0,170,255));
  daytime=true;
  }
  else if (current_time->tm_hour >= 5 && current_time->tm_hour < 12){
    window_set_background_color(window, GColorFromRGB(255,0,128));
    daytime=true;
  } 
  else if (current_time->tm_hour >= 17 && current_time->tm_hour < 21){
    window_set_background_color(window, GColorFromRGB(255,170,0));
    daytime=false;
  } 
  else if (current_time->tm_hour >= 21 || current_time->tm_hour < 5){
    window_set_background_color(window, GColorFromRGB(0,0,85));
    daytime=false;
  } 
}

static void inbox_received_callback(DictionaryIterator *iterator, void *context) {
  static char temperature_buffer[8];
  static char weather_layer_buffer[32];
  
  Tuple *t = dict_read_first(iterator);

  int temperature;
  int Kelvin = persist_read_int(KEY_TEMPERATURE);
  int finalTemp = Kelvin;
  int test = persist_read_int(KEY_TEST);
    
  while(t != NULL) {
    switch(t->key) {
    case KEY_SCALE:
      if(strcmp(t->value->cstring, "F") == 0){
        persist_write_int(KEY_TEST, 0);
        DictionaryIterator *iter;
        app_message_outbox_begin(&iter);
        dict_write_uint8(iter, 0, 0);
        app_message_outbox_send();
      }
      else if(strcmp(t->value->cstring, "C") == 0){
        persist_write_int(KEY_TEST, 1);
        DictionaryIterator *iter;
        app_message_outbox_begin(&iter);
        dict_write_uint8(iter, 0, 0);
        app_message_outbox_send();
      }
      break;
    case KEY_TEMPERATURE:
      if(test == 0){
      temperature = (int)t->value->int32;
      persist_write_int(KEY_TEMPERATURE, temperature);
      Kelvin = persist_read_int(KEY_TEMPERATURE);
      finalTemp = (Kelvin - 273.15) * 1.8 + 32;
      snprintf(temperature_buffer, sizeof(temperature_buffer), "%d°", finalTemp);
      }
      else if(test == 1){
      temperature = (int)t->value->int32;
      persist_write_int(KEY_TEMPERATURE, temperature);
      Kelvin = persist_read_int(KEY_TEMPERATURE);
      finalTemp = Kelvin - 273.15;
      snprintf(temperature_buffer, sizeof(temperature_buffer), "%d°", finalTemp);
      }
      break;
      
      case KEY_ICON:
      if (boss_bitmap) {
        gbitmap_destroy(boss_bitmap);
      }
      
      if(t->value->int32 == 0 && daytime==true){
      boss_bitmap = gbitmap_create_with_resource(BOSSES_IMAGE_RESOURCE_IDS[0]);
      }
      else if(t->value->int32 == 1){
      boss_bitmap = gbitmap_create_with_resource(BOSSES_IMAGE_RESOURCE_IDS[1]);
      }
      else if(t->value->int32 == 0 && daytime==false){
      boss_bitmap = gbitmap_create_with_resource(BOSSES_IMAGE_RESOURCE_IDS[2]);
      }
      bitmap_layer_set_bitmap(boss_layer, boss_bitmap);
      break;
    }

    t = dict_read_next(iterator);
  }
  
  snprintf(weather_layer_buffer, sizeof(weather_layer_buffer), "%s", temperature_buffer);
  text_layer_set_text(s_weather_layer, weather_layer_buffer);
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


static void update_display(struct tm *current_time) {
    if( ((current_time->tm_min == 0) && (current_time->tm_sec == 0)) || (initiate_watchface == true) ){ 

  if (initiate_watchface){
  start_number_image = (current_time->tm_sec) + (current_time->tm_min) + NUM_KIRBY;
}
    
  static long seed_images = 100;
  seed_images  = (((seed_images * 214013L + 2531011L) >> 16) & 32767);
  seed_images2 = seed_images + start_number_image;
  random_image = (seed_images2 % NUM_KIRBY);
  APP_LOG(APP_LOG_LEVEL_DEBUG, "random character generated [#%d].", random_image);

load_sequence();
set_container_image(&powers_images[1], powers_layers[1], POWERS_IMAGE_RESOURCE_IDS[random_image], GPoint(14, 26));
load_kirby_layer(window_get_root_layer(window));
  }
}

static void handle_minute_tick(struct tm *tick_time, TimeUnits units_changed)
{    
  update_time();
  update_bg_color(tick_time); 
  
  if(tick_time->tm_min % 30 == 0) {
    DictionaryIterator *iter;
    app_message_outbox_begin(&iter);
    dict_write_uint8(iter, 0, 0);
    app_message_outbox_send();
 }
  
  if(!initiate_watchface && tick_time->tm_min == 0 && tick_time->tm_hour == 0){ 
    load_sequence();
  }  
  update_display(tick_time);
}

void battery_state_handler(BatteryChargeState charge) {
	battery_level = charge.charge_percent;
  battery_plugged = charge.is_plugged;
  layer_mark_dirty(battery_layer);
}

static void handle_bluetooth(bool connected) {	
	if (connected) {

		if (!initiate_watchface) {
			vibes_double_pulse();
		}
	}
	else {

		if (!initiate_watchface) {      
			vibes_enqueue_custom_pattern( (VibePattern) {
   				.durations = (uint32_t []) {100, 100, 100, 100, 100},
   				.num_segments = 5
			} );
		}	
	}
}

static void handle_tap(AccelAxisType axis, int32_t direction)
{
  auto_hide = time(NULL) + 3;
  
  layer_set_hidden(text_layer_get_layer(text_date_layer), true);
  layer_set_hidden(text_layer_get_layer(s_weather_layer), false);
  
  animate = true;
  app_timer_register(10, timer_handler, NULL);
  app_timer_register(5000, stop_animation, NULL);
}

void handle_init(void) {
  
  time_t now = time(NULL);
 	struct tm *tick_time = localtime(&now);
   
  window = window_create();
  
  window_set_window_handlers(window, (WindowHandlers) {
    .load = main_window_load,
    .unload = main_window_unload,
  });   

	window_stack_push(window, true); 
  
  GRect dummy_frame = { {0, 0}, {0, 0} };
  
for (int i = 0; i < TOTAL_POWERS; ++i) {
   	powers_layers[i] = bitmap_layer_create(dummy_frame);
   	layer_add_child(window_get_root_layer(window), bitmap_layer_get_layer(powers_layers[i]));
}

	handle_minute_tick(tick_time, MINUTE_UNIT);
 	tick_timer_service_subscribe(MINUTE_UNIT, handle_minute_tick);
  
  battery_state_service_subscribe (&battery_state_handler);
  
  handle_bluetooth(bluetooth_connection_service_peek());
  bluetooth_connection_service_subscribe(&handle_bluetooth);
  
  accel_tap_service_subscribe(&handle_tap);
  app_timer_register(5000, stop_animation, NULL);
  
	APP_LOG(APP_LOG_LEVEL_DEBUG, "Just pushed a window!");
  
  initiate_watchface = false;
  
  update_bg_color(tick_time); 
  update_time();
  
  app_message_register_inbox_received(inbox_received_callback);
  app_message_register_inbox_dropped(inbox_dropped_callback);
  app_message_register_outbox_failed(outbox_failed_callback);
  app_message_register_outbox_sent(outbox_sent_callback);
  
  app_message_open(64,64); 
}

void handle_deinit(void) {  
	window_destroy(window);
}


int main(void) {
	handle_init();
	app_event_loop();
	handle_deinit();
}
