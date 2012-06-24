# encoding: utf-8

$:.unshift File.expand_path('lib', File.dirname(__FILE__))

require "rubygems"
require 'bundler'

require 'async_vips/version'

begin
  Bundler.setup(:default, :development)
rescue Bundler::BundlerError => e
  $stderr.puts e.message
  $stderr.puts "Run `bundle install` to install missing gems"
  exit e.status_code
end

require 'rake'
require 'jeweler'

Jeweler::Tasks.new do |gem|
  # gem is a Gem::Specification... see http://docs.rubygems.org/read/chapter/20 for more options

  gem.name = "async-ruby-vips"
  gem.homepage = "https://github.com/gchudnov/async-ruby-vips"
  gem.license = "MIT"
  
  gem.summary = %Q{async-ruby-vips is a ruby extension for vips that transforms images asynchronously.}
  
  gem.description = %Q{Non-blocking transformation and image writing in Ruby, powered by VIPS.}
  
  gem.email = "g.chudnov@gmail.com"
  gem.authors = ["Grigoriy Chudnov"]

  gem.version = AsyncVips::VERSION
  
  gem.files = [
    "*.*",
    "*.",
    "lib/**/*",
    "ext/**/*"
  ]

  # dependencies defined in Gemfile
end
Jeweler::RubygemsDotOrgTasks.new


task :default => :test

require 'rdoc/task'
Rake::RDocTask.new do |rdoc|
  version = File.exist?('VERSION') ? File.read('VERSION') : ""

  rdoc.rdoc_dir = 'rdoc'
  rdoc.title = "async-ruby-vips #{version}"
  rdoc.rdoc_files.include('README*')
  rdoc.rdoc_files.include('lib/**/*.rb')
end
