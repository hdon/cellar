#!/usr/bin/env python
import os, sys, gtk, gtk.glade, gobject
from gtk import gdk

class Application:
    def destroy(self, widget, data=None):
        gtk.main_quit()

    def delete_event(self, widget, event, data=None):
        return False

    def __init__(self):
        self.glade = gtk.glade.XML("gwireworld.glade")
        self.window = self.glade.get_widget('window1')
        self.status = self.glade.get_widget('statusbar1')
        self.window.show_all()

        self.glade.signal_autoconnect({
            'on_window1_delete_event':      self.delete_event,
            'on_window1_destroy':           self.destroy,
            'on_toggletoolbutton1_toggled': self.play_button_toggle,
        })

        # A "toolbox" gives users a palette of common CA patterns
        # A 'Store' abstracts our toolbox data from its view
        self.toolbox = gtk.ListStore(gobject.TYPE_STRING, gdk.Pixbuf)
        # A 'TreeView' provides the view of our toolbox
        toolbox_view = self.glade.get_widget('treeview1')
        # Associate our toolbox model and view
        toolbox_view.set_model(self.toolbox)
        # Teach our toolbox view how to use its model
        toolbox_view.append_column(gtk.TreeViewColumn('Name',
            gtk.CellRendererText(), text = 0))
        toolbox_view.append_column(gtk.TreeViewColumn('Preview',
            gtk.CellRendererPixbuf(), pixbuf = 1))

    def main(self):
        gtk.main()

    def play_button_toggle(self, widget):
        if widget.get_active():
            print 'playing'
        else:
            print 'pausing'

if __name__ == '__main__':
    try:
        app = Application()
        app.main()
    except KeyboardInterrupt:
        print 'bye'

