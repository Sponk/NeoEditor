setPluginName("TestPlugin2")
setPluginAuthor("Yannick Pflanzer")
setPluginDescription("Another simple Lua plugin calling some stuff for testing purposes")

addEditorMenu("Hello World", "hello_callback")

function hello_callback()
   path = openFileDlg("Select a file", os.getenv("HOME")) 
   messagebox("You selected " .. path)
end