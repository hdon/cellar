#!/usr/bin/env python
import os, sys, gtk, gtk.glade, gobject

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
        })

    def main(self):
        gtk.main()

if __name__ == '__main__':
    try:
        app = Application()
        app.main()
    except KeyboardInterrupt:
        print 'bye'

