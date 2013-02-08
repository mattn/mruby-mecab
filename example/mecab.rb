#!mruby

dict = ARGV[0] or raise "usage: [dict]"
mecab = Mecab.new("-d #{dict}")
mecab.sparse_toary("俺がいる").each do |x|
  x.split("\n").each do |l| 
    words = l.split("\t")
    break if words.size == 1
	tokens = words[1].split(",")
    puts "#{words[0]} #{tokens.join(",")}"
  end
end
