module Rake::DSL
  def submodule(module_name, rakefile_path)
    namespace module_name do
      load rakefile_path

      Rake::Task.tasks.select { |t| t.name.start_with?("#{module_name}:") }.each do |t|
        task t.name.sub("#{module_name}:", '') => "#{module_name}:chdir"
      end

      task :chdir do
        cd File.dirname(rakefile_path)
      end
    end
  end
end
