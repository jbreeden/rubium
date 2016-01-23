desc "Package the rubium app"
task :package do
  configuration = ENV['CONFIGURATION'] || 'Release'
  package(configuration)
end

if OS.mac?
  
  def package(configuration)
    app_dir = "rubium.app"
    app_name = "rubium"
    app_location = "mruby/bin/rubium"

    rm_rf app_dir if Dir.exists?(app_dir)
    cp_r "#{CEF.build_dir}/cefclient/#{configuration}/cefclient.app", app_dir
    Dir["#{app_dir}/Contents/Frameworks/cefclient Helper *"].each do |f|
      rm_rf f
    end
    Dir["#{app_dir}/Contents/**/cefclient*"].each do |f|
      rm_rf f if File.file?(f)
    end
    Dir["#{app_dir}/Contents/Frameworks/cefclient*"].each do |f|
      mv f, f.gsub('cefclient', app_name)
    end
    Dir["#{app_dir}/Contents/Frameworks/**/cefclient*"].each do |f|
      mv f, f.gsub('cefclient', app_name)
    end

    puts [app_location, "#{app_dir}/Contents/MacOS/#{app_name}"]
    cp app_location, "#{app_dir}/Contents/MacOS/#{app_name}"
    cp "#{app_location} Helper", "#{app_dir}/Contents/Frameworks/#{app_name} Helper.app/Contents/MacOS/#{app_name} Helper"
    Dir["**/#{app_name}*/**/Info.plist"].concat(Dir['Contents/Info.plist']).each do |plist|
      text = File.read(plist)
      File.open(plist, 'w') do |f|
        text.each_line do |line|
          f.write(line.gsub('cefclient', app_name))
        end
      end
    end
    sh "install_name_tool -change \"@executable_path/Chromium Embedded Framework\" \"@executable_path/../Frameworks/Chromium Embedded Framework.framework/Chromium Embedded Framework\" #{app_name}.app/Contents/MacOS/#{app_name}"
    sh "install_name_tool -change \"@executable_path/Chromium Embedded Framework\" \"@executable_path/../../../../Frameworks/Chromium Embedded Framework.framework/Chromium Embedded Framework\" \"#{app_name}.app/Contents/Frameworks/#{app_name} Helper.app/Contents/MacOS/#{app_name} Helper\""
    sh "BUILT_PRODUCTS_DIR=#{app_dir} CONTENTS_FOLDER_PATH=Contents #{CEF.dir}/tools/make_more_helpers.sh Frameworks #{app_name}"
  end
  
elsif OS.linux?
  
  def package(configuration)
    app_dir = "rubium"
    app_name = "rubium"
    app_location = "mruby/bin/rubium"

    rm_rf app_dir if Dir.exists?(app_dir)
    mkdir app_dir
    Dir["#{CEF.dir}/#{configuration}/*.{bin,so}"].concat(Dir["#{CEF.dir}/Resources/*"]).each do |file|
      cp_r file, app_dir
    end
    cp app_location, "#{app_dir}/#{app_name}"
    sh "chmod a+x #{app_dir}/#{app_name}"
  end

else

  def package(configuration)
    app_dir = "rubium"
    app_name = "rubium.exe"
    app_location = "mruby/bin/rubium.exe"

    rm_rf app_dir if Dir.exists?(app_dir)
    mkdir app_dir
    Dir["#{CEF.dir}/out/#{configuration}/*.{dll,pak,dat}"].each do |file|
      cp_r file, app_dir
    end
    mkdir "#{app_dir}/locales"
    Dir["#{CEF.dir}/out/#{configuration}/locales/*"].each do |file|
      cp_r file, "#{app_dir}/locales"
    end
    cp app_location, "#{app_dir}/#{app_name}"
  end

end
