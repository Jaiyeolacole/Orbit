# esp32-openai-chat

Minimal ESP-IDF chat client built on Espressif's [`openai`](https://docs.espressif.com/projects/esp-iot-solution/en/latest/ai/openai.html)
component from esp-iot-solution. Connects to Wi-Fi, then runs an interactive
chat loop over the serial console: type a line, press Enter, get a reply
from the Chat Completions API.

## Layout

```
.
├── CMakeLists.txt            top-level project file
├── sdkconfig.defaults
├── main/
│   ├── main.c                 app_main() + the chat REPL task
│   ├── config.h               shared, non-Kconfig declarations (log tag, buffer sizes, ...)
│   ├── Kconfig.projbuild       app-level menuconfig options
│   └── CMakeLists.txt
└── components/
    ├── wifi_manager/           blocking Wi-Fi station connect helper
    │   ├── include/wifi_manager.h
    │   ├── wifi_manager.c
    │   ├── Kconfig              SSID / password / retry count
    │   └── CMakeLists.txt
    └── chat_client/             thin wrapper around the `openai` component
        ├── include/chat_client.h
        ├── chat_client.c
        ├── Kconfig               API key, model, sampling params, etc.
        ├── idf_component.yml     declares the dependency on espressif/openai
        └── CMakeLists.txt
```

Every user-tunable value lives in a `Kconfig`/`Kconfig.projbuild` file and is
edited through `idf.py menuconfig`. `main/config.h` only holds declarations
that aren't meant to be end-user tunable (log tags, event bits, derived
constants) plus thin `#define`s that forward the relevant `CONFIG_xxx`
symbols so `main.c` doesn't sprinkle raw `CONFIG_` macros everywhere.

## Build

```sh
cd esp32-openai-chat
idf.py set-target esp32        # or esp32s3 / esp32c3 / ...
idf.py reconfigure             # fetches the openai component via the component manager
idf.py menuconfig
```

In menuconfig, set:

- **Wi-Fi Manager Configuration** → SSID, password, max retries
- **OpenAI Chat Client Configuration** → your API key (required), model,
  system prompt, max tokens, temperature, top-p, penalties, user id, and
  whether to keep conversation history between turns
- **Chat App Configuration** → Wi-Fi connect timeout, console line buffer
  size, and the chat task's stack size/priority

Then:

```sh
idf.py -p PORT flash monitor
```

Once connected, the console prints `you>` — type a message and press
Enter. `/reset` clears conversation history, `/quit` stops the chat task
(the device keeps running).

## Notes

- Kconfig has no float type, so temperature/top-p/penalties are configured
  as integers scaled by 100 (e.g. `70` → `0.70`) and divided back to a
  float in `chat_client.c`.
- The `openai` component is pulled in declaratively via
  `components/chat_client/idf_component.yml` (`espressif/openai: "*"`) —
  no manual `idf.py add-dependency` step needed, `idf.py reconfigure` (or
  the first build) fetches it into `managed_components/`.
- Chat Completions on this component is text-only for output (no
  streaming) as of this writing; a reply is returned in one shot once the
  HTTPS request completes.
- If your build errors while resolving the `openai` requirement, check the
  component's actual name under `managed_components/` after the first
  fetch and adjust `REQUIRES` in `components/chat_client/CMakeLists.txt`
  to match (this can shift between component-manager versions).
