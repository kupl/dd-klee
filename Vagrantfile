# -*- mode: ruby -*-
# vi: set ft=ruby :


Vagrant.configure("2") do |config|
  config.vagrant.plugins = ["vagrant-disksize", "vagrant-vbguest"]
  config.disksize.size = "20GB"
  config.vm.box = "ubuntu/bionic64"
  config.vm.hostname = "kupl"
  config.vm.define "dd-klee"
  
  config.vm.provider "virtualbox" do |vb|
    vb.name = "dd-klee"
    vb.memory = "2048"
    vb.cpus = "2"
    vb.customize ["modifyvm", :id, "--ioapic", "on"]
  end
  
  config.vm.provision "bootstrap", type: "shell",
      privileged: true, run: "always" do |bs|
    bs.path = "bootstrap.sh"
  end
  
  config.vm.provision "klee_deps", type: "shell",
      privileged: false, run: "never" do |kd|
    kd.path = "install_deps.sh"
  end
  
  config.vm.provision "klee", type: "shell",
      privileged: false, run: "never" do |k|
    k.path = "install_klee.sh"
  end

end
