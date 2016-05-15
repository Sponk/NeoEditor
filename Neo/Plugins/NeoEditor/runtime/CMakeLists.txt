find_package(Qt5Widgets)

if(not ${QT5_WIDGETS_FOUND})
    add_subdirectory(filedialog)
endif()