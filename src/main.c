#include <pebble.h>

#define KEY_TEMPERATURE 1
#define KEY_ICON 2
#define KEY_SCALE 3
#define KEY_SCALE_OPTION 4
#define KEY_STEPSGOAL 6

/*
Fire (Size 11512)
Beam (Size 4755)
Cutter (Size 4339)
Sword (Size 3515)
Hammer (Size 3345)
Mike (Size 2483)
Sleep (Size 1150)
*/

//Fix Auto Hide Issue
//Add animation for steps goal being reached
//Fix memory fragmentation

Window *window;

time_t auto_hide;

static bool initiate_watchface = true;
static bool animate = true;
static bool daytime;

AppTimer *weather_timeout;
static int timeout = 60000;

TextLayer *text_weather_layer;
TextLayer *text_time_layer;
TextLayer *text_date_layer;

static Layer *steps_layer;
int steps, steps_per_px, stepgoal;

static GBitmapSequence *s_sequence = NULL;

static uint8_t battery_level;
static bool battery_plugged;
static Layer *battery_layer;

static GBitmap *foreground_image;
static BitmapLayer *foreground_layer;

int replay = 2;
int current_frame, starting_frame;
int ending_frame;
int delay;//delay between each frame is in milliseconds

static int ANIMATIONS = 7;
int seed_images2, start_number_image, random_image;

#define TOTAL_POWERS 2
static GBitmap *powers_images[TOTAL_POWERS];
static BitmapLayer *powers_layers[TOTAL_POWERS];

#define TOTAL_BOSS 2
static GBitmap *boss_images[TOTAL_BOSS];
static BitmapLayer *boss_layers[TOTAL_BOSS];

#define TOTAL_KIRBY 2
static GBitmap *kirby_images[TOTAL_KIRBY];
static BitmapLayer *kirby_layers[TOTAL_KIRBY];

const int animation_frames[] = {  

RESOURCE_ID_KIRBY_BEAM_1, //0
RESOURCE_ID_KIRBY_BEAM_2,
RESOURCE_ID_KIRBY_BEAM_3,
RESOURCE_ID_KIRBY_BEAM_4,
RESOURCE_ID_KIRBY_BEAM_5,
RESOURCE_ID_KIRBY_BEAM_6,
RESOURCE_ID_KIRBY_BEAM_7,
RESOURCE_ID_KIRBY_BEAM_8,
RESOURCE_ID_KIRBY_BEAM_9,
RESOURCE_ID_KIRBY_BEAM_10,
RESOURCE_ID_KIRBY_BEAM_11,
RESOURCE_ID_KIRBY_BEAM_12,
RESOURCE_ID_KIRBY_BEAM_13, //12
  
RESOURCE_ID_KIRBY_CUTTER_1, //13
RESOURCE_ID_KIRBY_CUTTER_2,
RESOURCE_ID_KIRBY_CUTTER_3,
RESOURCE_ID_KIRBY_CUTTER_4,
RESOURCE_ID_KIRBY_CUTTER_5,
RESOURCE_ID_KIRBY_CUTTER_6,
RESOURCE_ID_KIRBY_CUTTER_7,
RESOURCE_ID_KIRBY_CUTTER_8,
RESOURCE_ID_KIRBY_CUTTER_9,
RESOURCE_ID_KIRBY_CUTTER_10,
RESOURCE_ID_KIRBY_CUTTER_11,
RESOURCE_ID_KIRBY_CUTTER_12,
RESOURCE_ID_KIRBY_CUTTER_13,
RESOURCE_ID_KIRBY_CUTTER_14,
RESOURCE_ID_KIRBY_CUTTER_15,
RESOURCE_ID_KIRBY_CUTTER_16,
RESOURCE_ID_KIRBY_CUTTER_17, //29
  
RESOURCE_ID_KIRBY_FIRE_1, //30
RESOURCE_ID_KIRBY_FIRE_2,
RESOURCE_ID_KIRBY_FIRE_3,
RESOURCE_ID_KIRBY_FIRE_4,
RESOURCE_ID_KIRBY_FIRE_5,
RESOURCE_ID_KIRBY_FIRE_6,
RESOURCE_ID_KIRBY_FIRE_7,
RESOURCE_ID_KIRBY_FIRE_8,
RESOURCE_ID_KIRBY_FIRE_9,
RESOURCE_ID_KIRBY_FIRE_10,
RESOURCE_ID_KIRBY_FIRE_11,
RESOURCE_ID_KIRBY_FIRE_12,
RESOURCE_ID_KIRBY_FIRE_13,
RESOURCE_ID_KIRBY_FIRE_14,
RESOURCE_ID_KIRBY_FIRE_15,
RESOURCE_ID_KIRBY_FIRE_16,
RESOURCE_ID_KIRBY_FIRE_17,
RESOURCE_ID_KIRBY_FIRE_18,
RESOURCE_ID_KIRBY_FIRE_19,
RESOURCE_ID_KIRBY_FIRE_20,
RESOURCE_ID_KIRBY_FIRE_21,
RESOURCE_ID_KIRBY_FIRE_22,
RESOURCE_ID_KIRBY_FIRE_23, //52
  
RESOURCE_ID_KIRBY_HAMMER_1, //53
RESOURCE_ID_KIRBY_HAMMER_2,
RESOURCE_ID_KIRBY_HAMMER_3,
RESOURCE_ID_KIRBY_HAMMER_4,
RESOURCE_ID_KIRBY_HAMMER_5,
RESOURCE_ID_KIRBY_HAMMER_6,
RESOURCE_ID_KIRBY_HAMMER_7,
RESOURCE_ID_KIRBY_HAMMER_8,
RESOURCE_ID_KIRBY_HAMMER_9,
RESOURCE_ID_KIRBY_HAMMER_10,
RESOURCE_ID_KIRBY_HAMMER_11,
RESOURCE_ID_KIRBY_HAMMER_12,
RESOURCE_ID_KIRBY_HAMMER_13, //65
  
RESOURCE_ID_KIRBY_MIKE_1, //66
RESOURCE_ID_KIRBY_MIKE_2,
RESOURCE_ID_KIRBY_MIKE_3,
RESOURCE_ID_KIRBY_MIKE_4,
RESOURCE_ID_KIRBY_MIKE_5,
RESOURCE_ID_KIRBY_MIKE_6,
RESOURCE_ID_KIRBY_MIKE_7,
RESOURCE_ID_KIRBY_MIKE_8,
RESOURCE_ID_KIRBY_MIKE_9,
RESOURCE_ID_KIRBY_MIKE_10,
RESOURCE_ID_KIRBY_MIKE_11,
RESOURCE_ID_KIRBY_MIKE_12,
RESOURCE_ID_KIRBY_MIKE_13,
RESOURCE_ID_KIRBY_MIKE_14, //79
  
RESOURCE_ID_KIRBY_SLEEP_1, //80
RESOURCE_ID_KIRBY_SLEEP_2,
RESOURCE_ID_KIRBY_SLEEP_3,
RESOURCE_ID_KIRBY_SLEEP_4,
RESOURCE_ID_KIRBY_SLEEP_5,
RESOURCE_ID_KIRBY_SLEEP_6, //85
  
RESOURCE_ID_KIRBY_SWORD_1, //86
RESOURCE_ID_KIRBY_SWORD_2,
RESOURCE_ID_KIRBY_SWORD_3,
RESOURCE_ID_KIRBY_SWORD_4,
RESOURCE_ID_KIRBY_SWORD_5,
RESOURCE_ID_KIRBY_SWORD_6,
RESOURCE_ID_KIRBY_SWORD_7,
RESOURCE_ID_KIRBY_SWORD_8,
RESOURCE_ID_KIRBY_SWORD_9, //94
};

const int POWERS_IMAGE_RESOURCE_IDS[] = 
{
  RESOURCE_ID_BEAM,
  RESOURCE_ID_CUTTER,
  RESOURCE_ID_FIRE,
  RESOURCE_ID_HAMMER,
  RESOURCE_ID_MIKE,
  RESOURCE_ID_SLEEP,
  RESOURCE_ID_SWORD,
};

const int BOSSES_IMAGE_RESOURCE_IDS[] = 
{
  RESOURCE_ID_MR_BRIGHT,
  RESOURCE_ID_KRACKO,
  RESOURCE_ID_MR_SHINE,
  RESOURCE_ID_KING,
};

static void load_foreground_layer(Layer *window_layer)
{
  foreground_image = gbitmap_create_with_resource(RESOURCE_ID_FOREGROUND);
  foreground_layer = bitmap_layer_create(layer_get_frame(window_layer));
  bitmap_layer_set_compositing_mode(foreground_layer, GCompOpSet);
  bitmap_layer_set_bitmap(foreground_layer, foreground_image);
 	layer_add_child(window_layer, bitmap_layer_get_layer(foreground_layer));
}

void step_layer_update_callback(Layer *layer, GContext *ctx) 
{
  stepgoal = persist_read_int(KEY_STEPSGOAL);
  steps_per_px = stepgoal / 50;
  graphics_context_set_compositing_mode(ctx, GCompOpAssign);
  GColor8 stepColor = GColorWhite;
  graphics_context_set_stroke_color(ctx, stepColor);
  graphics_context_set_fill_color(ctx,  stepColor);
  graphics_fill_rect(ctx, GRect(50 - (steps / steps_per_px), 0,(steps / steps_per_px), 10), 0, GCornerNone);
}

static void load_step_layer(Layer *window_layer)
{
 	steps_layer = layer_create(GRect(85,13,50,10));
 	layer_set_update_proc(steps_layer, &step_layer_update_callback);  
  layer_add_child(window_layer, steps_layer);
}

static void load_weather_layer(Layer *window_layer)
{
  text_weather_layer = text_layer_create(GRect(72, 130, 72, 26));
  text_layer_set_background_color(text_weather_layer, GColorClear);
  text_layer_set_text_color(text_weather_layer, GColorBlack);
  text_layer_set_text(text_weather_layer, "...");
  text_layer_set_font(text_weather_layer,fonts_get_system_font(FONT_KEY_LECO_20_BOLD_NUMBERS));
  text_layer_set_text_alignment(text_weather_layer, GTextAlignmentCenter);
  layer_add_child(window_layer, text_layer_get_layer(text_weather_layer));
  layer_set_hidden(text_layer_get_layer(text_weather_layer), true);
}

void battery_layer_update_callback(Layer *layer, GContext *ctx) 
{
  graphics_context_set_compositing_mode(ctx, GCompOpAssign);
  GColor8 batteryColor = GColorRed;
  graphics_context_set_stroke_color(ctx, batteryColor);
  graphics_context_set_fill_color(ctx,  batteryColor);
  graphics_fill_rect(ctx, GRect(0, 0, (uint8_t)(battery_level)/2, 10), 0, GCornerNone);
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

static void set_container_image(GBitmap **bmp_image, BitmapLayer *bmp_layer, const int resource_id, GPoint origin) 
{
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

static void timer_handler(void *context) 
{
  
  if(current_frame < ending_frame || replay > 0){
    if(current_frame == ending_frame){
     current_frame = starting_frame;
     replay--;
    }
    if (kirby_images[1] != NULL) {
      gbitmap_destroy(kirby_images[1]);
      kirby_images[1] = NULL;
    }
    
    kirby_images[1] = gbitmap_create_with_resource(animation_frames[current_frame]);
    
    bitmap_layer_set_bitmap(kirby_layers[1], kirby_images[1]);
    layer_mark_dirty(bitmap_layer_get_layer(kirby_layers[1]));

    current_frame++;
    app_timer_register(delay, timer_handler, NULL);
  }  
  
}


static void load_sequence() 
{
  //Beam
  if(random_image == 0){
  current_frame = 0;
  ending_frame = 13;
  delay = 77;
  }
  
  //Cutter
  if(random_image == 1){
  current_frame = 13;
  ending_frame = 30;
  delay = 59;
  }
  
  //Fire
  if(random_image == 2){
  current_frame = 31;
  ending_frame = 52;
  delay = 88;
  }
  
  //Hammer
  if(random_image == 3){
  current_frame = 53;
  ending_frame = 66;
  delay = 77;
  }
  
  //Mike
  if(random_image == 4){
  current_frame = 67;
  ending_frame = 80;
  delay = 72;
  }
  
  //Sleep
  if(random_image == 5){
  current_frame = 81;
  ending_frame = 86;
  delay = 167;
  }
  
  //Sword
  if(random_image == 6){
  current_frame = 87;
  ending_frame = 95;
  delay = 111;
  }

  starting_frame = current_frame;
  app_timer_register(1, timer_handler, NULL);
}

static void load_kirby_layer()
{ 
  if(random_image == 0){//Beam
   set_container_image(&kirby_images[1], kirby_layers[1], animation_frames[0], GPoint(0, 54));
  }
  else if(random_image == 1){//Cutter
   set_container_image(&kirby_images[1], kirby_layers[1], animation_frames[13], GPoint(0, 81));
  }
  else if(random_image == 2){//Fire
    set_container_image(&kirby_images[1], kirby_layers[1], animation_frames[30], GPoint(0, 66));
  }
  else if(random_image == 3){//Hammer
    set_container_image(&kirby_images[1], kirby_layers[1], animation_frames[53], GPoint(5, 69));
  }
  else if(random_image == 4){//Mike
    set_container_image(&kirby_images[1], kirby_layers[1], animation_frames[66], GPoint(0, 80));
  }
  else if(random_image == 5){//Sleep
    set_container_image(&kirby_images[1], kirby_layers[1], animation_frames[80], GPoint(13, 75));
  }
  else if(random_image == 6){//Sword
    set_container_image(&kirby_images[1], kirby_layers[1], animation_frames[86], GPoint(0, 69));
  }

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

static void update_bg_color(struct tm *current_time) 
{
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

static void weather_ended() 
{
	// If the weather can't be updated show the error icon
	//APP_LOG(APP_LOG_LEVEL_INFO, "Weather timer ended");
	
	if (weather_timeout != NULL) {
		//APP_LOG(APP_LOG_LEVEL_INFO, "Weather timer is not NULL");
    set_container_image(&boss_images[1], boss_layers[1], BOSSES_IMAGE_RESOURCE_IDS[3], GPoint(82, 53));
	}
}

static void cancel_weather_timeout() 
{
	// Cancel the timeout once weather is received
	if (weather_timeout != NULL) {
			//APP_LOG(APP_LOG_LEVEL_INFO, "Cancelling weather timer");
			app_timer_cancel(weather_timeout);
			weather_timeout = NULL;
	}
}

static void inbox_received_callback(DictionaryIterator *iterator, void *context) 
{
  static char temperature_buffer[8];
  static char weather_layer_buffer[32];
  
  Tuple *t = dict_read_first(iterator);

  int temperature;
  int finalTemp;
  int scale_option = persist_read_int(KEY_SCALE_OPTION);
  
  while(t != NULL) {
    switch(t->key) {
    case KEY_SCALE:
      if(strcmp(t->value->cstring, "F") == 0){
        persist_write_int(KEY_SCALE_OPTION, 0);
        DictionaryIterator *iter;
        app_message_outbox_begin(&iter);
        dict_write_uint8(iter, 0, 0);
        app_message_outbox_send();
      }
      else if(strcmp(t->value->cstring, "C") == 0){
        persist_write_int(KEY_SCALE_OPTION, 1);
        DictionaryIterator *iter;
        app_message_outbox_begin(&iter);
        dict_write_uint8(iter, 0, 0);
        app_message_outbox_send();
      }
      break;
    case KEY_TEMPERATURE:
      
      if(scale_option == 0){
      temperature = t->value->int32;
      finalTemp = (temperature - 273.15) * 1.8 + 32;
      snprintf(temperature_buffer, sizeof(temperature_buffer), "%d°", finalTemp);
      }
      else if(scale_option == 1){
      temperature = t->value->int32;
      finalTemp = temperature - 273.15;
      snprintf(temperature_buffer, sizeof(temperature_buffer), "%d°", finalTemp);
      }
      break;
      
      case KEY_ICON:
      
      cancel_weather_timeout();
      
      if(t->value->int32 == 0 && daytime==true){
      set_container_image(&boss_images[1], boss_layers[1], BOSSES_IMAGE_RESOURCE_IDS[0], GPoint(94, 56));
      }
      else if(t->value->int32 == 1){
      set_container_image(&boss_images[1], boss_layers[1], BOSSES_IMAGE_RESOURCE_IDS[1], GPoint(64, 36));
      }
      else if(t->value->int32 == 0 && daytime==false){
      set_container_image(&boss_images[1], boss_layers[1], BOSSES_IMAGE_RESOURCE_IDS[2], GPoint(97, 69));
      }
      break;
    
      case KEY_STEPSGOAL:
      
      stepgoal = t->value->int16;
		  //APP_LOG(APP_LOG_LEVEL_INFO, "stepgoal is %d", stepgoal);
		  persist_write_int(KEY_STEPSGOAL, stepgoal);
      break;
  }

    t = dict_read_next(iterator);
  }
  
  snprintf(weather_layer_buffer, sizeof(weather_layer_buffer), "%s", temperature_buffer);
  text_layer_set_text(text_weather_layer, weather_layer_buffer);
}

static void inbox_dropped_callback(AppMessageResult reason, void *context) 
{
  //APP_LOG(APP_LOG_LEVEL_ERROR, "Message dropped!");
}

static void outbox_failed_callback(DictionaryIterator *iterator, AppMessageResult reason, void *context) 
{
  //APP_LOG(APP_LOG_LEVEL_ERROR, "Outbox send failed!");
}

static void outbox_sent_callback(DictionaryIterator *iterator, void *context) 
{
  //APP_LOG(APP_LOG_LEVEL_INFO, "Outbox send success!");
}

static void update_display(struct tm *current_time) 
{
  start_number_image = (current_time->tm_sec) + (current_time->tm_min) + ANIMATIONS;

  static long seed_images = 100;
  seed_images  = (((seed_images * 214013L + 2531011L) >> 16) & 32767);
  seed_images2 = seed_images + start_number_image;
  random_image = (seed_images2 % ANIMATIONS);
  //APP_LOG(APP_LOG_LEVEL_DEBUG, "random character generated [#%d].", random_image);

  load_sequence();
  set_container_image(&powers_images[1], powers_layers[1], POWERS_IMAGE_RESOURCE_IDS[random_image], GPoint(14, 26));
  load_kirby_layer();
  update_bg_color(current_time);
}

static void handle_minute_tick(struct tm *tick_time, TimeUnits units_changed)
{    
  if( (units_changed & SECOND_UNIT) != 0 ) {
    if(time(NULL) == auto_hide){
      layer_set_hidden(text_layer_get_layer(text_date_layer), false);
      layer_set_hidden(text_layer_get_layer(text_weather_layer), true);
    } 
  }
  
  if( (units_changed & MINUTE_UNIT) != 0 ) {

    update_time();
    
    if(tick_time->tm_min % 30 == 0) {
    DictionaryIterator *iter;
    app_message_outbox_begin(&iter);
    dict_write_uint8(iter, 0, 0);
    app_message_outbox_send();
    weather_timeout = app_timer_register(timeout, weather_ended, NULL);
    }
 }
  
    if( ((tick_time->tm_min == 0) && (tick_time->tm_sec == 0)) || (initiate_watchface == true) ){
      update_display(tick_time);
    }
}

static void handle_health(HealthEventType event, void *context) 
{
	time_t start = time_start_of_today();
	time_t end = time(NULL);
	HealthServiceAccessibilityMask mask = health_service_metric_accessible(HealthMetricStepCount, start, end);
		
		if (mask & HealthServiceAccessibilityMaskAvailable) {
				//APP_LOG(APP_LOG_LEVEL_INFO, "Step data available!");
				steps = health_service_sum_today(HealthMetricStepCount);
				//APP_LOG(APP_LOG_LEVEL_INFO, "Steps: %d", steps);
		} else {
				//APP_LOG(APP_LOG_LEVEL_INFO, "Step data unavailable");
		}
}

void handle_battery(BatteryChargeState charge) 
{
	battery_level = charge.charge_percent;
  battery_plugged = charge.is_plugged;
  layer_mark_dirty(battery_layer);
}

static void handle_bluetooth(bool connected) 
{	
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
  replay = 2;
  load_sequence();
  auto_hide = time(NULL) + 4;
  layer_set_hidden(text_layer_get_layer(text_date_layer), true);
  layer_set_hidden(text_layer_get_layer(text_weather_layer), false);
}

static void main_window_load(Window *window) 
{
  Layer *window_layer = window_get_root_layer(window);
    
  load_foreground_layer(window_layer);
  load_step_layer(window_layer);
  load_weather_layer(window_layer);
  load_battery_layer(window_layer);
  load_time_text_layer(window_layer);
  load_date_text_layer(window_layer);   
}

static void main_window_unload(Window *window) 
{  
   gbitmap_destroy(foreground_image);
   bitmap_layer_destroy(foreground_layer);
  
   layer_destroy(steps_layer);
  
   text_layer_destroy(text_weather_layer);
  
   layer_destroy(battery_layer);
  
   text_layer_destroy(text_time_layer);
  
   text_layer_destroy(text_date_layer);
  
   for (int i = 0; i < TOTAL_POWERS; i++) {
     	layer_remove_from_parent(bitmap_layer_get_layer(powers_layers[i]));
     	gbitmap_destroy(powers_images[i]);
     	bitmap_layer_destroy(powers_layers[i]);
 	 }
  
   for (int i = 0; i < TOTAL_KIRBY; i++) {
     	layer_remove_from_parent(bitmap_layer_get_layer(kirby_layers[i]));
     	gbitmap_destroy(kirby_images[i]);
     	bitmap_layer_destroy(kirby_layers[i]);
 	 }  
  
   gbitmap_sequence_destroy(s_sequence);
  
   for (int i = 0; i < TOTAL_BOSS; i++) {
     	layer_remove_from_parent(bitmap_layer_get_layer(boss_layers[i]));
     	gbitmap_destroy(boss_images[i]);
     	bitmap_layer_destroy(boss_layers[i]);
 	 }
}

void handle_init(void) 
{
  window = window_create();
  
  window_set_window_handlers(window, (WindowHandlers) {
    .load = main_window_load,
    .unload = main_window_unload,
  });   
    
  time_t now = time(NULL);
 	struct tm *tick_time = localtime(&now);

	window_stack_push(window, true); 
  
  GRect dummy_frame = { {0, 0}, {0, 0} };
  
for (int i = 0; i < TOTAL_POWERS; ++i) {
   	powers_layers[i] = bitmap_layer_create(dummy_frame);
   	layer_add_child(window_get_root_layer(window), bitmap_layer_get_layer(powers_layers[i]));
    bitmap_layer_set_compositing_mode(powers_layers[i], GCompOpSet);
}
  
for (int i = 0; i < TOTAL_BOSS; ++i) {
   	boss_layers[i] = bitmap_layer_create(dummy_frame);
   	layer_add_child(window_get_root_layer(window), bitmap_layer_get_layer(boss_layers[i]));
    bitmap_layer_set_compositing_mode(boss_layers[i], GCompOpSet);
}
  
  for (int i = 0; i < TOTAL_KIRBY; ++i) {
   	kirby_layers[i] = bitmap_layer_create(dummy_frame);
   	layer_add_child(window_get_root_layer(window), bitmap_layer_get_layer(kirby_layers[i]));
    bitmap_layer_set_compositing_mode(kirby_layers[i], GCompOpSet);
}

	handle_minute_tick(tick_time, MINUTE_UNIT);
  
 	tick_timer_service_subscribe(MINUTE_UNIT | SECOND_UNIT, handle_minute_tick);
  
  health_service_events_subscribe(handle_health, NULL);
  battery_state_service_subscribe (&handle_battery);
  
  handle_bluetooth(bluetooth_connection_service_peek());
  bluetooth_connection_service_subscribe(&handle_bluetooth);
  
  accel_tap_service_subscribe(&handle_tap);
  
	//APP_LOG(APP_LOG_LEVEL_DEBUG, "Just pushed a window!");
  
  initiate_watchface = false;
  
  update_bg_color(tick_time); 
  update_time();
  
  app_message_register_inbox_received(inbox_received_callback);
  app_message_register_inbox_dropped(inbox_dropped_callback);
  app_message_register_outbox_failed(outbox_failed_callback);
  app_message_register_outbox_sent(outbox_sent_callback);
  
  app_message_open(64,64); 
}

void handle_deinit(void) 
{  
	window_destroy(window);
  battery_state_service_unsubscribe();
  accel_tap_service_unsubscribe();
  bluetooth_connection_service_unsubscribe();
  bluetooth_connection_service_peek();
  tick_timer_service_unsubscribe();
  health_service_events_unsubscribe();
}

int main(void) 
{
	handle_init();
	app_event_loop();
	handle_deinit();
}