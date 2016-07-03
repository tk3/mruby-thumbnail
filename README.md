# mruby-thumbnail

Thumbnail class extension.

## Setting

### Setting build_config.rb

```rb
MRuby::Build.new do |conf|
  ...
  conf.gem :github => 'qtkmz/mruby-thumbnail'
  ...
end
```

## Usage

### Image resize to 50% of original size

```rb
thumb = Thumbnail.new

thumb.load_file "./IMG_0001.jpg"

original_width = thumb.width
original_height = thumb.height

thumb.resize original_width/2, original_height/2

thumb.write_file "./out.jpg"
```

# License
The MIT License (MIT)

Copyright (c) 2016 qtakamitsu

