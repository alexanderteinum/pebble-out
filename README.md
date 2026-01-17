# Out

[![License: MIT](https://img.shields.io/badge/License-MIT-yellow.svg)](https://opensource.org/licenses/MIT)
[![Build](https://github.com/alexanderteinum/pebble-out/actions/workflows/build.yml/badge.svg)](https://github.com/alexanderteinum/pebble-out/actions/workflows/build.yml)

Out is an opinionated, minimalist weather dashboard for Pebble. It tries to answer three specific questions:

1. Do I need an umbrella?
2. Is there a storm coming?
3. Do I need sunscreen?

![Screenshot](./screenshot.png)

## Features

* **Smart Precipitation Logic:** Instead of showing raw data, Out calculates the probability of rain or thunder occurring at least once within the next 3 hours.
* **UV Index:** Displays the peak UV index predicted for the next 3 hours.
* **Battery:** A line is along the edge of the screen indicating the battery level.
* **Zero Configuration:** Uses the phone’s geolocation automatically. No settings page needed.
* **High Contrast:** Optimized for the Memory LCD display using standard system fonts (LECO and Gothic).

## Data Source

Weather data is provided by The Norwegian Meteorological Institute (Met.no) via their Locationforecast 2.0 API. This is the same API that Yr uses, and it works all over the world.

## Build From Source

Ensure you have the Pebble SDK installed.

```bash
# Build the project
pebble build

# Install to watch
pebble install --cloudpebble
```

## License

Distributed under the MIT License. See [LICENSE](LICENSE) for more information.
