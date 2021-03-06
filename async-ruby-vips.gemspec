# Generated by jeweler
# DO NOT EDIT THIS FILE DIRECTLY
# Instead, edit Jeweler::Tasks in Rakefile, and run 'rake gemspec'
# -*- encoding: utf-8 -*-
# stub: async-ruby-vips 1.2.1 ruby lib
# stub: ext/extconf.rb

Gem::Specification.new do |s|
  s.name = "async-ruby-vips"
  s.version = "1.2.1"

  s.required_rubygems_version = Gem::Requirement.new(">= 0") if s.respond_to? :required_rubygems_version=
  s.require_paths = ["lib"]
  s.authors = ["Grigoriy Chudnov"]
  s.date = "2014-01-27"
  s.description = "Non-blocking transformation and image writing in Ruby, powered by VIPS."
  s.email = "g.chudnov@gmail.com"
  s.extensions = ["ext/extconf.rb"]
  s.extra_rdoc_files = [
    "README.md"
  ]
  s.files = [
    "Gemfile.lock",
    "README.md",
    "async-ruby-vips.gemspec",
    "ext/LICENSE",
    "ext/async_vips.c",
    "ext/async_vips.h",
    "ext/callback.c",
    "ext/callback.h",
    "ext/details.c",
    "ext/details.h",
    "ext/extconf.rb",
    "ext/image.c",
    "ext/image.h",
    "ext/info.c",
    "ext/info.h",
    "ext/transform.c",
    "ext/transform.h",
    "ext/transform_data.c",
    "ext/transform_data.h",
    "ext/transform_data_fwd.h",
    "ext/writer.c",
    "ext/writer.h",
    "lib/async_vips.rb",
    "lib/async_vips/version.rb"
  ]
  s.homepage = "https://github.com/gchudnov/async-ruby-vips"
  s.licenses = ["MIT"]
  s.rubygems_version = "2.2.1"
  s.summary = "async-ruby-vips is a ruby extension for vips that transforms images asynchronously."

  if s.respond_to? :specification_version then
    s.specification_version = 4

    if Gem::Version.new(Gem::VERSION) >= Gem::Version.new('1.2.0') then
      s.add_development_dependency(%q<rdoc>, ["~> 3.12"])
      s.add_development_dependency(%q<bundler>, ["~> 1.5"])
      s.add_development_dependency(%q<jeweler>, ["~> 1.8"])
      s.add_development_dependency(%q<rspec>, ["~> 2.14"])
    else
      s.add_dependency(%q<rdoc>, ["~> 3.12"])
      s.add_dependency(%q<bundler>, ["~> 1.5"])
      s.add_dependency(%q<jeweler>, ["~> 1.8"])
      s.add_dependency(%q<rspec>, ["~> 2.14"])
    end
  else
    s.add_dependency(%q<rdoc>, ["~> 3.12"])
    s.add_dependency(%q<bundler>, ["~> 1.5"])
    s.add_dependency(%q<jeweler>, ["~> 1.8"])
    s.add_dependency(%q<rspec>, ["~> 2.14"])
  end
end

