# Custom Audio
To replace a sequences you should only create a zip file with sequences to replace at the same path as mk64.o2r. Only wav, mp3, ogg and flac are supported. For define the loop you need to create a json with the same name but end with .json.
the json look like this:
```json
{
    "loop": {
        "start": 0,
        "end": 0
    }
}
```
The json file are optional. The unit of `start` and `end` is in pcm frames. You can use a tool like Audacity to find the correct values (it's call samples in Audacity). All properties avaible are:
```json
{
    "name": "Name",
    "author": "Author",
    "date": "Date",
    "loop": {
        "start": 0,
        "end": 0
    }
}
```
The `name`, `author` and `date` are optional. The `loop` is used to define the loop of the sequence. If you don't want to loop the sequence, you can just remove the `loop` property. `date` doesn't have any effect on the audio playback.
## Example:
You want to change sound/sequences/main_menu then you just need to create a zip file with the following structure:
```
audio_pack.zip
└── sound
    └── sequences
        └── main_menu.mp3
        └── main_menu.json
```

## Future plans
* Make name more meaningful and uniform. (example: change `seq_16`). You can suggest name or make a PR to help rename them/organise them.
* Support samples.
* Create a tool to convert sequences and samples.