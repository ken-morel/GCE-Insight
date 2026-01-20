#include "gce_data.h"

#include "visualization.h"
#include <assets/data/data_sample.csv.h>
#include <gama.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Utility functions
static double map_value(double value, double in_min, double in_max,
                        double out_min, double out_max) {
  return (value - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
}

// Visualization drawing functions - only defined in main.c where gama.h is
// included
void draw_visualization() {
  // Draw chart area
  gm_draw_rectangle(vis_data.left, vis_data.top, vis_data.right - vis_data.left,
                    vis_data.bottom - vis_data.top, gm_set_alpha(GM_WHITE, 10));

  // Draw axes
  // X-axis (Year)
  gm_draw_line(vis_data.left, 0, vis_data.right, 0, 0.005, GM_GRAY);

  // Y-axis (Pass Percentage)
  gm_draw_line(0, vis_data.bottom, 0, vis_data.top, 0.005, GM_GRAY);

  // Draw axis labels
  if (vis_data.data && vis_data.data->record_count > 0) {
    // X-axis labels (Years)
    int start_year = vis_data.data->records[0].year;
    int end_year = vis_data.data->records[vis_data.data->record_count - 1].year;

    for (int year = start_year; year <= end_year; year++) {
      if (year % 2 == 0) { // Label every other year to avoid crowding
        double x_pos = map_value(year, start_year, end_year, vis_data.left,
                                 vis_data.right);

        char year_str[10];
        sprintf(year_str, "%d", year);

        // Vertical line for each year
        gm_draw_line(x_pos, vis_data.bottom, x_pos, vis_data.top, 0.002,
                     gm_set_alpha(GM_WHITE, 30));

        // Year label
        gm_draw_text(x_pos, vis_data.bottom - 0.08, year_str, "", 0.03,
                     GM_GRAY);
      }
    }

    // Y-axis labels (Pass %)
    for (float percent = 0; percent <= 100; percent += 20) {
      double y_pos = map_value(percent, 0, 100, vis_data.bottom, vis_data.top);

      // Horizontal line for each percentage
      if (percent > 0 && percent < 100) {
        gm_draw_line(vis_data.left, y_pos, vis_data.right, y_pos, 0.002,
                     gm_set_alpha(GM_WHITE, 30));
      }

      char percent_str[10];
      sprintf(percent_str, "%.1f%%", percent);

      // Percentage label
      gm_draw_text(vis_data.left - 0.08, y_pos, percent_str, "", 0.03, GM_GRAY);
    }
  }

  // Axis titles
  gm_draw_text(0, vis_data.bottom - 0.15, "Year", "", 0.05, GM_WHITE);
  gm_draw_text(vis_data.left - 0.18, 0, "Pass %", "", 0.05, GM_WHITE);

  // Draw data points if enabled
  if (vis_data.show_data_points && vis_data.data) {
    int start_year = vis_data.data->records[0].year;
    int end_year = vis_data.data->records[vis_data.data->record_count - 1].year;

    for (size_t i = 0; i < vis_data.data->record_count; i++) {
      // Map year to x position
      double x = map_value(vis_data.data->records[i].year, start_year, end_year,
                           vis_data.left, vis_data.right);

      // Map pass percentage to y position
      double y = map_value(vis_data.data->records[i].pass_percent, 0, 100,
                           vis_data.bottom, vis_data.top);

      // Draw the data point
      gm_draw_circle(x, y, 0.015, GM_CYAN);

      // Optionally, draw year labels near the points
      if (i % 2 == 0) { // Label every other point to avoid crowding
        char year_str[10];
        sprintf(year_str, "%d", vis_data.data->records[i].year);
        gm_draw_text(x, y - 0.04, year_str, "", 0.025, GM_WHITE);
      }
    }
  }

  // Draw predictions if enabled
  if (vis_data.show_predictions && vis_data.data &&
      vis_data.data->record_count > 0) {
    // Get year range for mapping
    int start_year = vis_data.data->records[0].year;
    int end_year = vis_data.data->records[vis_data.data->record_count - 1].year;

    // Draw the predicted point for 2026
    double pred_x = map_value(vis_data.prediction_year, start_year, end_year,
                              vis_data.left, vis_data.right);

    double pred_y = map_value(vis_data.predicted_pass_percent, 0, 100,
                              vis_data.bottom, vis_data.top);

    // Draw the prediction point
    gm_draw_circle(pred_x, pred_y, 0.02, GM_RED);

    // Draw a line from last actual data point to the prediction
    GCE_Subject_Record last_record =
        vis_data.data->records[vis_data.data->record_count - 1];

    double last_x = map_value(last_record.year, start_year, end_year,
                              vis_data.left, vis_data.right);

    double last_y = map_value(last_record.pass_percent, 0, 100, vis_data.bottom,
                              vis_data.top);

    gm_draw_line(last_x, last_y, pred_x, pred_y, 0.007,
                 gm_set_alpha(GM_RED, 150));

    // Draw prediction label
    char pred_str[30];
    sprintf(pred_str, "%d: %.2f%%", vis_data.prediction_year,
            vis_data.predicted_pass_percent);
    gm_draw_text(pred_x, pred_y + 0.05, pred_str, "", 0.03, GM_RED);
  }

  // Draw legend
  // Draw a legend box in the upper right of the chart
  double legend_x = vis_data.right - 0.25;
  double legend_y = vis_data.top + 0.05;

  // Background box
  gm_draw_rectangle(legend_x - 0.01, legend_y - 0.01, 0.22, 0.15,
                    gm_set_alpha(GM_BLACK, 180));
  // Draw outline using lines
  double x1 = legend_x - 0.01;
  double y1 = legend_y - 0.01;
  double x2 = x1 + 0.22;
  double y2 = y1 + 0.15;
  gm_draw_line(x1, y1, x2, y1, 0.003, GM_WHITE); // top
  gm_draw_line(x2, y1, x2, y2, 0.003, GM_WHITE); // right
  gm_draw_line(x2, y2, x1, y2, 0.003, GM_WHITE); // bottom
  gm_draw_line(x1, y2, x1, y1, 0.003, GM_WHITE); // left

  // Legend items
  double item_y = legend_y;
  gm_draw_circle(legend_x, item_y, 0.01, GM_CYAN);
  gm_draw_text(legend_x + 0.07, item_y, "Actual Data", "", 0.03, GM_CYAN);

  item_y -= 0.04;
  gm_draw_circle(legend_x, item_y, 0.01, GM_RED);
  gm_draw_text(legend_x + 0.07, item_y, "Prediction", "", 0.03, GM_RED);

  item_y -= 0.04;
  gm_draw_line(legend_x - 0.02, item_y, legend_x + 0.02, item_y, 0.007, GM_RED);
  gm_draw_text(legend_x + 0.07, item_y, "Trend Line", "", 0.03, GM_RED);
}

void draw_data_info() {
  // Draw overall information about the data
  char info_str[200];
  sprintf(info_str, "GCE Computer Science (0795) Predictor | Records: %zu",
          vis_data.data ? vis_data.data->record_count : 0);
  gm_draw_text(0, 0.9, info_str, "", 0.04, GM_WHITE);

  // Draw prediction information
  if (vis_data.predicted_pass_percent > 0) {
    char pred_info[100];
    sprintf(pred_info, "Predicted %d Pass %%: %.2f%%", vis_data.prediction_year,
            vis_data.predicted_pass_percent);
    gm_draw_text(0, 0.85, pred_info, "", 0.04, GM_YELLOW);
  }

  // Draw controls information
  gm_draw_text(
      0, -0.9,
      "Controls: [R] Reset, [S] Show/Hide Data, [P] Show/Hide Predictions", "",
      0.03, gm_set_alpha(GM_WHITE, 200));
}

int main() {
  // Initialize Gama engine, use 0,0 size for web support
  gm_init(0, 0, "GCE Insight - Predicting GCE Advanced Level Performance");

  size_t gce_data_size = 0;
  const unsigned char *gce_data = data_sample_data(&gce_data_size);

  gm_background(GM_BLACK);
  gapi_fullscreen(0); // Set to 0 for windowed mode, 1 for fullscreen

  // Initialize data structures
  GCEDataCollection *data_collection = init_gce_data_collection();

  // Load sample data
  parse_gce_data(data_collection, gce_data,
                 gce_data_size); // Note: file parameter is currently ignored in
                                 // our implementation

  printf("Loaded csv file:\n");
  size_t n_records = data_collection->record_count;
  for (size_t i = 0; i < n_records; i++) {
    GCE_Subject_Record *r = &data_collection->records[i];
    printf("%d\t%d\t%d\t%d\t%d\t%d\t%d\t%d\t%d\t%d\t%d\n", r->year,
           r->registered, r->sat, r->absent, r->fail, r->grade_A, r->grade_B,
           r->grade_C, r->grade_D, r->grade_E, r->grade_F);
  }

  // Initialize visualization
  init_visualization();

  // Main application loop
  do {
    // Check for user input
    if (gm_key('r')) { // Reset on 'R' key
      // Reload data
      cleanup_gce_data_collection(data_collection);
      data_collection = init_gce_data_collection();
      parse_gce_data(data_collection, gce_data, gce_data_size);
    }
    if (gm_key('s')) // Toggle data points on 'S' key
      vis_data.show_data_points = !vis_data.show_data_points;

    if (gm_key('p')) // Toggle predictions on 'P' key
      vis_data.show_predictions = !vis_data.show_predictions;

    // Update and draw visualization
    update_visualization(data_collection);
    draw_visualization();
    draw_data_info();
  } while (gm_yield());

  // Cleanup
  cleanup_gce_data_collection(data_collection);

  return 0;
}
