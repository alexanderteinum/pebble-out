#include <pebble.h>

static Window *s_main_window;

static Layer *s_battery_layer;
static TextLayer *s_temp_label, *s_temp_value;
static TextLayer *s_uv_label, *s_uv_value;
static TextLayer *s_rain_label, *s_rain_value;
static TextLayer *s_thunder_label, *s_thunder_value;
static GFont s_custom_font;

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
  Tuple *temp_tuple = dict_find(iterator, MESSAGE_KEY_Temp);
  if(temp_tuple) {
    int temp = (int)temp_tuple->value->int32;
    static char s_temp_buffer[16];
    if (temp == -99) {
      snprintf(s_temp_buffer, sizeof(s_temp_buffer), "-");
    } else {
      if (!clock_is_24h_style()) {
        temp = (temp * 9 / 5) + 32;
        snprintf(s_temp_buffer, sizeof(s_temp_buffer), "%d°F", temp);
      } else {
        snprintf(s_temp_buffer, sizeof(s_temp_buffer), "%d°C", temp);
      }
    }
    text_layer_set_text(s_temp_value, s_temp_buffer);
  }

  Tuple *uv_tuple = dict_find(iterator, MESSAGE_KEY_UV);
  if(uv_tuple) {
    static char s_uv_buffer[8];
    int uv = (int)uv_tuple->value->int32;
    if (uv == -1) {
      snprintf(s_uv_buffer, sizeof(s_uv_buffer), "-");
    } else {
      snprintf(s_uv_buffer, sizeof(s_uv_buffer), "%d", uv);
    }
    text_layer_set_text(s_uv_value, s_uv_buffer);
  }

  Tuple *rain_tuple = dict_find(iterator, MESSAGE_KEY_Rain);
  if(rain_tuple) {
    static char s_rain_buffer[8];
    int rain = (int)rain_tuple->value->int32;
    if (rain == -1) {
      snprintf(s_rain_buffer, sizeof(s_rain_buffer), "-");
    } else {
      snprintf(s_rain_buffer, sizeof(s_rain_buffer), "%d%%", rain);
    }
    text_layer_set_text(s_rain_value, s_rain_buffer);
  }

  Tuple *thunder_tuple = dict_find(iterator, MESSAGE_KEY_Thunder);
  if(thunder_tuple) {
    static char s_thunder_buffer[8];
    int thunder = (int)thunder_tuple->value->int32;
    if (thunder == -1) {
      snprintf(s_thunder_buffer, sizeof(s_thunder_buffer), "-");
    } else {
      snprintf(s_thunder_buffer, sizeof(s_thunder_buffer), "%d%%", thunder);
    }
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

static TextLayer* create_row_layer(GRect bounds, char* text, GTextAlignment alignment) {
  TextLayer *layer = text_layer_create(bounds);
  text_layer_set_text(layer, text);
  text_layer_set_font(layer, s_custom_font);
  text_layer_set_text_alignment(layer, alignment);
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

  int row_h = 28;
  int margin = 10;
#ifdef PBL_ROUND
  margin = 24;
#endif
  int y_pos = (bounds.size.h - (row_h * 4)) / 2;
  int available_w = bounds.size.w - (margin * 2);
  int label_w = (available_w * 6) / 10;
  int value_w = available_w - label_w;

  s_temp_label = create_row_layer(GRect(margin, y_pos, label_w, row_h), "Temp", GTextAlignmentLeft);
  layer_add_child(window_layer, text_layer_get_layer(s_temp_label));
  s_temp_value = create_row_layer(GRect(margin + label_w, y_pos, value_w, row_h), "-", GTextAlignmentRight);
  layer_add_child(window_layer, text_layer_get_layer(s_temp_value));
  y_pos += row_h;

  s_uv_label = create_row_layer(GRect(margin, y_pos, label_w, row_h), "UV", GTextAlignmentLeft);
  layer_add_child(window_layer, text_layer_get_layer(s_uv_label));
  s_uv_value = create_row_layer(GRect(margin + label_w, y_pos, value_w, row_h), "-", GTextAlignmentRight);
  layer_add_child(window_layer, text_layer_get_layer(s_uv_value));
  y_pos += row_h;

  s_rain_label = create_row_layer(GRect(margin, y_pos, label_w, row_h), "Rain", GTextAlignmentLeft);
  layer_add_child(window_layer, text_layer_get_layer(s_rain_label));
  s_rain_value = create_row_layer(GRect(margin + label_w, y_pos, value_w, row_h), "-", GTextAlignmentRight);
  layer_add_child(window_layer, text_layer_get_layer(s_rain_value));
  y_pos += row_h;

  s_thunder_label = create_row_layer(GRect(margin, y_pos, label_w, row_h), "Thunder", GTextAlignmentLeft);
  layer_add_child(window_layer, text_layer_get_layer(s_thunder_label));
  s_thunder_value = create_row_layer(GRect(margin + label_w, y_pos, value_w, row_h), "-", GTextAlignmentRight);
  layer_add_child(window_layer, text_layer_get_layer(s_thunder_value));
}

static void window_unload(Window *window) {
  layer_destroy(s_battery_layer);
  text_layer_destroy(s_temp_label);
  text_layer_destroy(s_temp_value);
  text_layer_destroy(s_uv_label);
  text_layer_destroy(s_uv_value);
  text_layer_destroy(s_rain_label);
  text_layer_destroy(s_rain_value);
  text_layer_destroy(s_thunder_label);
  text_layer_destroy(s_thunder_value);
}

static void init(void) {
#if defined(PBL_PLATFORM_EMERY) || defined(PBL_PLATFORM_GABBRO)
  s_custom_font = fonts_load_custom_font(resource_get_handle(RESOURCE_ID_FONT_INTER_20));
#else
  s_custom_font = fonts_load_custom_font(resource_get_handle(RESOURCE_ID_FONT_INTER_16));
#endif
  
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
  fonts_unload_custom_font(s_custom_font);
}

int main(void) {
  init();
  app_event_loop();
  deinit();
}
