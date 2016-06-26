MRuby::Gem::Specification.new('mruby-thumbnail') do |spec|
  spec.license = 'MIT'
  spec.author  = 'qtakamitsu <qtakamitsu@gmail.com>'
  spec.summary = 'Thumbnail class extension'

  spec.cc.flags << `pkg-config --cflags MagickWand`.chomp.split(" ")
  magickwand_libs = `pkg-config --libs MagickWand`.chomp.split(" ")
  spec.linker.library_paths << magickwand_libs.map{|i| i.match(/^-L(.+)/){|md| md[1]}}.compact
  spec.linker.libraries << magickwand_libs.map{|i| i.match(/^-l(.+)/){|md| md[1]}}.compact
end
