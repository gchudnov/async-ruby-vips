async-ruby-vips
===============

Non-blocking transformation and image writing in Ruby, powered by VIPS

## Requirements.

  * OS X or Linux
  * MRI 1.9.2
  * libvips 7.28.x

## Example.

```ruby
require 'rubygems'
require 'async_vips'
include AsyncVips


# Non-blocking scaling of an image:
# Allowed symbols are:  :load, :scale_x, :scale_y, :save
# Wneh called, a function is invoked on a separate native thread. 
# The callback-proc is invoked on a separate Ruby thread on completion.
AsyncVips.transform(:load => srcfile, :scale_x => 800, :save => dstfile) do |img|
  if(img.error != nil)
    puts img.error
  end
end

# Set the number of operations cached by VIPS: 
AsyncVips.set_cache(100)

# Get image information:
# Allowed symbols are:  :load
AsyncVips.info(:load => srcfile) do |img|
  puts img.src    # image filepath
  puts img.width  # image width
  puts img.height # image height
  puts img.size   # image size in bytes
end


## Using event machine

require 'rubygems'
require 'async_vips'
require 'eventmachine'
include AsyncVips

#puts AsyncVips::VERSION
#puts AsyncVips::LIB_VERSION
AsyncVips.set_cache(0) # sets the number of operations cached by VIPS

EM.run do

  EM.add_timer(3) do
     source = './images/*.JPG'
     files = Dir[source]
     files.each do |srcfile|
       dstfile = './output/'+File.basename(srcfile)
       
       AsyncVips.transform(:load => srcfile, :scale_x => 800, :save => dstfile) do |img|
         puts img.src
         if(img.error != nil)
           puts img.error
         end                 
       end
        
     end
  end

  EM.add_periodic_timer(1) do
    puts "#{Time.now}"
  end
end


```ruby
