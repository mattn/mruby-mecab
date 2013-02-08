MRuby::Gem::Specification.new('mruby-mecab') do |spec|
  spec.license = 'MIT'
  spec.authors = 'mattn'

  spec.linker.libraries << ['mecab']
end
