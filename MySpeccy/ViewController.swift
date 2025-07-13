//      Copyright © 2025  Andrius Mazeikis
//
//      This program is free software: you can redistribute it and/or modify
//      it under the terms of the GNU General Public License as published by
//      the Free Software Foundation, either version 3 of the License, or
//      (at your option) any later version.
//
//      This program is distributed in the hope that it will be useful,
//      but WITHOUT ANY WARRANTY; without even the implied warranty of
//      MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//      GNU General Public License for more details.
//
//      You should have received a copy of the GNU General Public License
//      along with this program.  If not, see <https://www.gnu.org/licenses/>.
//

import Cocoa
import MetalKit
import AudioToolbox

class ViewController: NSViewController {
    
    var renderer: Renderer!
    var machine = Machine()
    var audio: Audio!
    
    let keyLettersToCodes: Dictionary<String, UInt32> = [
        "z": 0x02, "x": 0x04, "c": 0x08, "v": 0x10,
        "a": 0x21, "s": 0x22, "d": 0x24, "f": 0x28, "g": 0x30,
        "q": 0x41, "w": 0x42, "e": 0x44, "r": 0x48, "t": 0x50,
        "1": 0x61, "2": 0x62, "3": 0x64, "4": 0x68, "5": 0x70,
        "!": 0x61, "@": 0x62, "#": 0x64, "$": 0x68, "%": 0x70,
        "0": 0x81, "9": 0x82, "8": 0x84, "7": 0x88, "6": 0x90,
        ")": 0x81, "(": 0x82, "*": 0x84, "&": 0x88, "^": 0x90,
        "p": 0xA1, "o": 0xA2, "i": 0xA4, "u": 0xA8, "y": 0xB0,
        "\r":0xC1,"l": 0xC2, "k": 0xC4, "j": 0xC8, "h": 0xD0,
        " ": 0xE1, "m": 0xE4, "n": 0xE8, "b": 0xF0
    ]

    @IBOutlet weak var mtkView: MTKView!
    
    override func viewDidLoad() {
        super.viewDidLoad()
        
        NSEvent.addLocalMonitorForEvents(matching: .keyDown) { self.keyDown(with: $0) }
        NSEvent.addLocalMonitorForEvents(matching: .keyUp) { self.keyUp(with: $0) }
        NSEvent.addLocalMonitorForEvents(matching: .flagsChanged) { self.keyFlagsChanged(with: $0); return $0 }
        
//        guard let url = Bundle.main.url(forResource: "48k-2", withExtension: "rom"),
//              let data = try? Data(contentsOf: url) else { return }
//        
//        data.withUnsafeBytes {
//            self.machine.loadROM($0.bindMemory(to: UInt8.self).baseAddress!, UInt32($0.count))
//        }
      
        let frameInfo = machine.frameInfo();

        renderer = Renderer.create(for: mtkView, width: Int(frameInfo.width), height: Int(frameInfo.height), bytesPerRow: Int(frameInfo.bytesPerRow))
        
        audio = Audio { buffer in
            var frameData = FrameData()
            let bufferCapacity = Int(buffer.pointee.mAudioDataBytesCapacity) / MemoryLayout<Float>.size
            frameData.audioBuffer.buffer = buffer.pointee.mAudioData.bindMemory(to: Float.self, capacity: bufferCapacity)
            frameData.audioBuffer.capacity = UInt32(bufferCapacity)
            self.machine.processFrame(&frameData)
            self.renderer.updateImage(bytes: frameData.pixels)
            buffer.pointee.mAudioDataByteSize = frameData.audioSamplesProduced * UInt32(MemoryLayout<Float>.size)
        }
        
        audio.start()

    }
    
    func keyDown(with event: NSEvent) -> NSEvent? {
        guard let key = event.charactersIgnoringModifiers?.lowercased() else {
            return event
        }
        
        if let keycode = keyLettersToCodes[key] {
            machine.keyDown(keycode);
            return nil
        }
        return event
    }
    
    func keyUp(with event: NSEvent) -> NSEvent? {
        guard let key = event.charactersIgnoringModifiers?.lowercased() else {
            return event
        }

        if let keycode = keyLettersToCodes[key] {
            machine.keyUp(keycode);
            return nil
        }
        return event
    }
    
    func keyFlagsChanged(with event: NSEvent) {
        if (event.modifierFlags.intersection(.shift) == .shift) {
            machine.keyDown(0x01)
        } else {
            machine.keyUp(0x01)
        }
        if (event.modifierFlags.intersection(.option) == .option) {
            machine.keyDown(0xE2)
        } else {
            machine.keyUp(0xE2)
        }
    }

    override var representedObject: Any? {
        didSet {
        // Update the view, if already loaded.
        }
    }
}

