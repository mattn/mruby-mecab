MRuby::Gem::Specification.new('mruby-mecab') do |spec|
  spec.license = 'MIT'
  spec.authors = 'mattn'

  #spec.cc.flags << '-I"#{ENV["libuv_path"] + "/include"}"'
  spec.linker.libraries << ['mecab']
end
