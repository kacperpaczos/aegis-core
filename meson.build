project('aegis-core', 'cpp',
    version: '1.0',
    default_options: ['cpp_std=c++17']
)

cpp = meson.get_compiler('cpp')

# Konfiguracja serwera
conf_data = configuration_data()
conf_data.set('version', '1.0.0')
conf_data.set('port', '8443')
conf_data.set('threads', '4')
conf_data.set('max_connections', '100')
conf_data.set('queue_size', '50')
conf_data.set('ssl_enabled', 'false')
conf_data.set('log_level', 'information')
conf_data.set('working_dir', '/var/lib/aegis-core')

# Zależności POCO
poco_deps = []
poco_libs = ['PocoFoundation', 'PocoNet', 'PocoJSON', 'PocoUtil']

foreach lib : poco_libs
    poco_deps += cpp.find_library(lib, required: true)
endforeach

# Generowanie pliku konfiguracyjnego
configure_file(
    input: 'include/config.h.in',
    output: 'config.h',
    configuration: conf_data
)

inc = include_directories(['include', 'include/api'])

executable('aegis-core',
    sources: [
        'src/main.cpp',
        'src/http_server.cpp',
        'src/api/core_api.cpp',
        'src/api/profile_api.cpp',
        'src/api/faceent_api.cpp',
        'src/api/camera_api.cpp',
        'src/api/pam_api.cpp'
    ],
    include_directories: inc,
    dependencies: poco_deps
)