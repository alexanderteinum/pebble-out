#include <pebble.h>

static Window *s_main_window;

static Layer *s_battery_layer;
static TextLayer *s_rain_label;
static TextLayer *s_rain_value;
static TextLayer *s_thunder_label;
static TextLayer *s_thunder_value;
static TextLayer *s_uv_label;
static TextLayer *s_uv_value;

static int s_battery_level = 0;

static void battery_update_proc(Layer *layer, GContext *ctx) {
  GRect bounds = layer_get_bounds(layer);
  
#ifdef PBL_ROUND
  int thickness = 6;
  GRect ring_bounds = grect_inset(bounds, GEdgeInsets(thickness / 2));

  graphics_context_set_fill_color(ctx, GColorLightGray);
  graphics_fill_radial(ctx, ring_bounds, GOvalScaleModeFitCircle, thickness, 0, DEG_TO_TRIGANGLE(360));

  graphics_context_set_fill_color(ctx, GColorBlack);
  int angle = DEG_TO_TRIGANGLE(360 * s_battery_level / 100);
  graphics_fill_radial(ctx, ring_bounds, GOvalScaleModeFitCircle, thickness, DEG_TO_TRIGANGLE(-90), DEG_TO_TRIGANGLE(-90) + angle);

#else
  graphics_context_set_fill_color(ctx, GColorLightGray);
  graphics_fill_rect(ctx, GRect(0, 0, bounds.size.w, 4), 0, GCornerNone);

  graphics_context_set_fill_color(ctx, GColorBlack);
  int width = (s_battery_level * bounds.size.w) / 100;
  graphics_fill_rect(ctx, GRect(0, 0, width, 4), 0, GCornerNone);
#endif
}

static void handle_battery(BatteryChargeState charge_state) {
  s_battery_level = charge_state.charge_percent;
  layer_mark_dirty(s_battery_layer);
}

static void inbox_received_callback(DictionaryIterator *iterator, void *context) {
  Tuple *uv_tuple = dict_find(iterator, MESSAGE_KEY_UV);
  if(uv_tuple) {
    static char s_uv_buffer[8];
    snprintf(s_uv_buffer, sizeof(s_uv_buffer), "%d", (int)uv_tuple->value->int32);
    text_layer_set_text(s_uv_value, s_uv_buffer);
  }

  Tuple *rain_tuple = dict_find(iterator, MESSAGE_KEY_Rain);
  if(rain_tuple) {
    static char s_rain_buffer[8];
    snprintf(s_rain_buffer, sizeof(s_rain_buffer), "%d%%", (int)rain_tuple->value->int32);
    text_layer_set_text(s_rain_value, s_rain_buffer);
  }

  Tuple *thunder_tuple = dict_find(iterator, MESSAGE_KEY_Thunder);
  if(thunder_tuple) {
    static char s_thunder_buffer[8];
    snprintf(s_thunder_buffer, sizeof(s_thunder_buffer), "%d%%", (int)thunder_tuple->value->int32);
    text_layer_set_text(s_thunder_value, s_thunder_buffer);
  }
}

static void request_weather(void) {
  DictionaryIterator *iter;
  app_message_outbox_begin(&iter);
  if (!iter) return;
  int value = 1;
  dict_write_int(iter, MESSAGE_KEY_UV, &value, sizeof(int), true);
  app_message_outbox_send();
}

static void inbox_dropped_callback(AppMessageResult reason, void *context) { APP_LOG(APP_LOG_LEVEL_ERROR, "Message dropped!"); }
static void outbox_failed_callback(DictionaryIterator *iterator, AppMessageResult reason, void *context) { APP_LOG(APP_LOG_LEVEL_ERROR, "Outbox failed!"); }
static void outbox_sent_callback(DictionaryIterator *iterator, void *context) { APP_LOG(APP_LOG_LEVEL_INFO, "Outbox success!"); }

static TextLayer* create_label_layer(GRect bounds, char* text) {
  TextLayer *layer = text_layer_create(bounds);
  text_layer_set_text(layer, text);
  text_layer_set_font(layer, fonts_get_system_font(FONT_KEY_GOTHIC_18_BOLD));
  text_layer_set_text_alignment(layer, GTextAlignmentCenter);
  text_layer_set_background_color(layer, GColorClear);
  return layer;
}

static TextLayer* create_value_layer(GRect bounds) {
  TextLayer *layer = text_layer_create(bounds);
  text_layer_set_text(layer, "-");
  text_layer_set_font(layer, fonts_get_system_font(FONT_KEY_LECO_28_LIGHT_NUMBERS)); 
  text_layer_set_text_alignment(layer, GTextAlignmentCenter);
  text_layer_set_background_color(layer, GColorClear);
  return layer;
}

static void window_load(Window *window) {
  Layer *window_layer = window_get_root_layer(window);
  GRect bounds = layer_get_bounds(window_layer);

#ifdef PBL_ROUND
  s_battery_layer = layer_create(bounds);
#else
  s_battery_layer = layer_create(GRect(0, 0, bounds.size.w, 4));
#endif
  layer_set_update_proc(s_battery_layer, battery_update_proc);
  layer_add_child(window_layer, s_battery_layer);

  int row1_h = 20 + 30;
  int row2_h = 20 + 45;
  
  int spacer = 15;      
  int top_margin = 0;
  int side_inset = 0; 

#ifdef PBL_ROUND
  top_margin = 14;
  side_inset = 20;
  spacer = 5;
#endif

  int total_content_height = row1_h + spacer + row2_h;
  int start_y_centered = (bounds.size.h - total_content_height) / 2;
  int y_pos = start_y_centered + top_margin;
  
  int col_w = bounds.size.w / 2;

  s_rain_label = create_label_layer(GRect(side_inset, y_pos, col_w - side_inset, 20), "RAIN");
  layer_add_child(window_layer, text_layer_get_layer(s_rain_label));
  
  s_rain_value = create_value_layer(GRect(side_inset, y_pos + 18, col_w - side_inset, 35));
  text_layer_set_font(s_rain_value, fonts_get_system_font(FONT_KEY_GOTHIC_28_BOLD)); 
  layer_add_child(window_layer, text_layer_get_layer(s_rain_value));

  s_thunder_label = create_label_layer(GRect(col_w, y_pos, col_w - side_inset, 20), "THUNDER");
  layer_add_child(window_layer, text_layer_get_layer(s_thunder_label));

  s_thunder_value = create_value_layer(GRect(col_w, y_pos + 18, col_w - side_inset, 35));
  text_layer_set_font(s_thunder_value, fonts_get_system_font(FONT_KEY_GOTHIC_28_BOLD));
  layer_add_child(window_layer, text_layer_get_layer(s_thunder_value));

  y_pos = y_pos + row1_h + spacer; 

  s_uv_label = create_label_layer(GRect(0, y_pos, bounds.size.w, 20), "UV INDEX");
  layer_add_child(window_layer, text_layer_get_layer(s_uv_label));

  s_uv_value = create_value_layer(GRect(0, y_pos + 20, bounds.size.w, 50));
  text_layer_set_font(s_uv_value, fonts_get_system_font(FONT_KEY_LECO_42_NUMBERS)); 
  layer_add_child(window_layer, text_layer_get_layer(s_uv_value));
}

static void window_unload(Window *window) {
  layer_destroy(s_battery_layer);
  text_layer_destroy(s_rain_label);
  text_layer_destroy(s_rain_value);
  text_layer_destroy(s_thunder_label);
  text_layer_destroy(s_thunder_value);
  text_layer_destroy(s_uv_label);
  text_layer_destroy(s_uv_value);
}

static void init(void) {
  s_main_window = window_create();
  window_set_window_handlers(s_main_window, (WindowHandlers) { .load = window_load, .unload = window_unload });
  window_stack_push(s_main_window, true);

  app_message_register_inbox_received(inbox_received_callback);
  app_message_open(128, 128);

  battery_state_service_subscribe(handle_battery);
  handle_battery(battery_state_service_peek());

  request_weather();
}

static void deinit(void) {
  window_destroy(s_main_window);
  battery_state_service_unsubscribe();
}

int main(void) {
  init();
  app_event_loop();
  deinit();
}
