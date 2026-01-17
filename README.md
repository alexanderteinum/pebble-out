# Out

**An opinionated, minimalist weather dashboard for Pebble.**

It tries to answer three specific questions:

1. Do I need an umbrella?
2. Is there a storm coming?
3. Do I need sunscreen?

![Screenshot of Out running on Pebble Time](./screenshot.png)

## Features

* **Smart Precipitation Logic:** Instead of showing raw data, Type calculates the probability of rain or thunder occurring *at least once* within the next 6 hours.
* **UV Index:** Displays the peak UV index predicted for the next 6 hours.
* **Hardware Empathy:**
    * **Square (Pebble Time/Steel):** Features a discreet battery bar at the top.
    * **Round (Pebble Time Round):** Features a custom-drawn radial progress ring that hugs the bezel.
* **Zero Configuration:** Uses the phone's geolocation automatically. No settings page needed.
* **High Contrast:** Optimized for the Memory LCD display using standard system fonts (LECO and Gothic).

## Data Source

Weather data is provided by **The Norwegian Meteorological Institute (Met.no)** via their Locationforecast 2.0 API.

> Note: This application uses the `complete` timeseries to calculate aggregate probabilities client-side on the phone before sending a lightweight payload to the watch.

## Build & Installation

    ```bash
    pebble build
    pebble install --cloudpebble
    ```

## Project Structure

* `src/c/main.c`: Handles the UI layout, drawing primitives (battery ring/bar), and AppMessage reception.
* `src/pkjs/index.js`: Fetches JSON from Met.no, performs probability math, and sends simplified integers to the watch.

## License

[MIT](LICENSE) © 2026 Alexander Teinum
