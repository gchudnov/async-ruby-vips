ENV['RC_ARCHS'] = '' if RUBY_PLATFORM =~ /darwin/

require "mkmf"

File::unlink("Makefile") if (File::exist?("Makefile"))

if not pkg_config("vips")
  VIPS_VERSIONS = %w[7.29 7.28 7.27 7.26 7.24]

  if not VIPS_VERSIONS.detect {|x| pkg_config("vips-#{x}") }
    raise("no pkg_config for any of following libvips versions: #{VIPS_VERSIONS.join(', ')}")
  end
end

have_header('vips/vips.h')
have_header('libexif/exif-data.h')

create_makefile('async_vips_ext')
