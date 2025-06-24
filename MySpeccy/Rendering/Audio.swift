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

import AudioToolbox

class Audio {

    typealias BufferFiller = (AudioQueueBufferRef) -> Void

    let outputQueue: AudioQueueRef
    
    struct QueueData {
        let bufferFiller: BufferFiller
    }
    
    let queueData: UnsafeMutablePointer<QueueData>
    
    init?(_ handler: @escaping BufferFiller) {
        var bufferRef: AudioQueueRef?
        var format = AudioStreamBasicDescription(mSampleRate: 44100, mFormatID: kAudioFormatLinearPCM, mFormatFlags: kLinearPCMFormatFlagIsFloat | kAudioFormatFlagIsPacked, mBytesPerPacket: 4, mFramesPerPacket: 1, mBytesPerFrame: 4, mChannelsPerFrame: 1, mBitsPerChannel: 32, mReserved: 0)
//        queueData = QueueData(bufferFiller: handler)
        queueData = UnsafeMutablePointer<QueueData>.allocate(capacity: 1)
        queueData.initialize(to: QueueData(bufferFiller: handler))
        let result = AudioQueueNewOutput(&format, outputQueueCallback, queueData, CFRunLoopGetCurrent(), nil, 0, &bufferRef)
        if result != 0 { return nil }
        outputQueue = bufferRef!
        for _ in 0..<3 {
            var buffer: AudioQueueBufferRef?
            let result = AudioQueueAllocateBuffer(outputQueue, 4096, &buffer)
            if result != 0 { return nil }
            buffer?.pointee.mAudioData.initializeMemory(as: Float32.self, repeating: 0.0, count: 1024)
            buffer?.pointee.mAudioDataByteSize = 4096
            AudioQueueEnqueueBuffer(outputQueue, buffer!, 0, nil)
        }
    }
    
    func start() {
        AudioQueueStart(outputQueue, nil)
    }
    
    func stop() {
        AudioQueueStop(outputQueue, true)
    }
    
    deinit {
        AudioQueueStop(outputQueue, true)
        AudioQueueDispose(outputQueue, true)
        queueData.deinitialize(count: 1)
        queueData.deallocate()
    }
}

func outputQueueCallback(_ data: UnsafeMutableRawPointer?, _ queue: AudioQueueRef, _ buffer: AudioQueueBufferRef) {
    data?.assumingMemoryBound(to: Audio.QueueData.self).pointee.bufferFiller(buffer)
    AudioQueueEnqueueBuffer(queue, buffer, 0, nil)
}
