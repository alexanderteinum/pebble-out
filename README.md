# Out

[![License: MIT](https://img.shields.io/badge/License-MIT-yellow.svg)](https://opensource.org/licenses/MIT)
[![Build](https://github.com/alexanderteinum/pebble-out/actions/workflows/build.yml/badge.svg)](https://github.com/alexanderteinum/pebble-out/actions/workflows/build.yml)

Out is an open source, minimalist weather dashboard for Pebble, built for clarity and fast decisions.

![Screenshot](screenshot.png)

## Features

- **3-Hour Window:** Shows the highest predicted risk of rain, thunder, and UV exposure for the next three hours.
- **Current Temperature:** Displays the real-time temperature in Celsius or Fahrenheit based on your watch settings.
- **Inter Typography:** Uses the Inter typeface for a modern, refined look.
- **Zero Configuration:** Automatically uses your phone’s geolocation. No settings or setup required.
- **Battery Awareness:** A subtle indicator along the edge of the screen shows your remaining charge.

## Data Source

Weather data is provided by MET Norway (The Norwegian Meteorological Institute). It provides high-quality, global forecasts used by services like Yr.

## Build From Source

Ensure you have the Pebble SDK installed.

```bash
# Build the project
pebble build

# Install to watch
pebble install --cloudpebble
```

## Credits

- **Inter Font:** Designed by Rasmus Andersson. Licensed under the [SIL Open Font License, Version 1.1](https://scripts.sil.org/OFL).
- **Weather Data:** Provided by [MET Norway](https://www.met.no/).
- **Screenshot Device Frame:** From the Rebble App Store.

## License

Distributed under the MIT License. See [LICENSE](LICENSE) for more information.
