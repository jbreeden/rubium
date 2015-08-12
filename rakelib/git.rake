def in_each_repo(&block)
  %w(
    .
    ./mrbgems/mruby-apr
    ./mruby
  ).each do |component_dir|
      Dir.chdir(component_dir, &block)
    end
end

namespace :git do
  desc "Check status of all component repos"
  task :status do
    in_each_repo do
        puts
        puts '---'
        puts "Git status: #{File.expand_path Dir.pwd}"
        puts '---'
        sh "git status"
    end
  end

  desc "Run `git diff` in every repo"
  task :diff do
    in_each_repo do
        puts
        puts '---'
        puts "Git diff: #{File.expand_path Dir.pwd}"
        puts '---'
        sh "git diff"
    end
  end

  desc "Run `git diff` in every repo"
  task :diff_cached do
    in_each_repo do
        puts
        puts '---'
        puts "Git diff: #{File.expand_path Dir.pwd}"
        puts '---'
        sh "git diff --cached"
    end
  end

  desc "Run `git add -u` in every repo"
  task :update do
    in_each_repo do
        puts
        puts '---'
        puts "Git update: #{File.expand_path Dir.pwd}"
        puts '---'
        sh "git add -u"
    end
  end

  desc "Run `git add -u` in every repo"
  task :commit do
    in_each_repo do
        puts
        puts '---'
        puts "Git commit: #{File.expand_path Dir.pwd}"
        puts '---'
        next if `git status` =~ /nothing to commit/i
        print 'Commit message: '
        msg = $stdin.gets.strip
        sh "git commit -m \"#{msg}\" || echo NOTHING COMMITTED"
    end
  end

  desc "Run `git push` in every repo"
  task :push do
    in_each_repo do
        puts
        puts '---'
        puts "Git push: #{File.expand_path Dir.pwd}"
        puts '---'
        sh "git status"
        print 'remote: '
        remote = $stdin.gets.strip
        print 'branch: '
        branch = $stdin.gets.strip
        sh "git push #{remote} #{branch} || echo NOTHING PUSHED"
    end
  end
end
