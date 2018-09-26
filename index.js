let SerialPort = require('serialport')
let argv = require('minimist')(process.argv.slice(2))
let fs = require('fs')
let _ = require('lodash')
let Promise = require('promise')

if (argv.l) {
    SerialPort.list().then(list => {
        list.forEach(port => {
            _.keys(port).forEach(key => {
                console.log(`${key}: ${port[key]}`)
            })
        })
        process.exit()
    })
    return
}

if (!argv.p) {
    console.error('Error: No port specified')
    process.exit(1)
}

let portName = argv.p
let baudRate = 9600

if (!argv.i) {
    console.error('Error: No input file supplied')
    process.exit(1)
}

let speedMultiplier = 1

if (argv.s) {
    speedMultiplier = parseFloat(argv.s)
}

let file = {}

// let buffer = fs.readFileSync(argv.i)

// /**
//  * 
//  * @param {number} offset 
//  * @param {number} number 
//  * @returns {Buffer}
//  */
// function getBytes(offset, number) {
//     return buffer.slice(offset, number)
// }

// /**
//  * 
//  * @param {number} offset 
//  * @returns {Buffer}
//  */
// function getByte(offset) {
//     return getBytes(offset, 1)
// }

// if (getBytes(0x00, 4).toString() != 'Vgm ') {
//     console.error('File is not a vgm file')
//     process.exit(1)
// }

let port = new SerialPort(portName, {
    baudRate: baudRate,
    // autoOpen: false
})

// const Delimiter = require('@serialport/parser-delimiter')
// const parser = port.pipe(new Delimiter({ delimiter: '\n' }))
// parser.on('data', console.log)

// port.on('error', function (data) {
//     console.log('Error: ' + data)
// })

// // Switches the port into "flowing mode"
// port.on('data', function (data) {
//     console.log('Echo: ', data);
// });

port.on('open', () => {
    setTimeout(() => {

        console.log(`Port '${portName}' with baud rate ${baudRate} has been opened.`)

        fs.open(argv.i, 'r', (status, fd) => {
            if (status) {
                console.error(status.message)
                return
            }

            function readByte(fd, offset) {
                var buffer = Buffer.alloc(1)
                fs.readSync(fd, buffer, 0, 1, offset)
                return buffer[0]
                // fs.read(fd, buffer, 0, buffer.length, offset,
                //     (err, num) => {
                //         console.log(buffer.toString('utf8', 0, num));
                //     }
                // );
            }

            /**
             * 
             * @param {*} fd 
             * @param {number} offset 
             * @param {number} number 
             * @returns {Buffer}
             */
            function readBytes(fd, offset, number) {
                var buffer = Buffer.alloc(number)
                fs.readSync(fd, buffer, 0, buffer.length, offset)
                return buffer
                // fs.read(fd, buffer, 0, buffer.length, offset,
                //     (err, num) => {
                //         console.log(buffer.toString('utf8', 0, num));
                //     }
                // );
            }

            if (readBytes(fd, 0x00, 4).toString() != 'Vgm ') {
                console.error('File is not a vgm file')
                return;
            }

            // port.write(fs.readFileSync(argv.i))

            let major = readByte(fd, 0x09).toString(16)
            let minor = readByte(fd, 0x08).toString(16)
            file.Version = { major, minor }

            file.SN76489Clock = readBytes(fd, 0x0C, 4).readInt32LE()

            // VGM data offset is it's value + header
            // address to compensate for relative value
            file.DataOffset = readBytes(fd, 0x34, 4).readInt32LE() + 0x34

            file.EoFOffset = readBytes(fd, 0x04, 4).readInt32LE() + 0x04

            file.LoopOffset = readBytes(fd, 0x1C, 4).readInt32LE() + 0x1C

            file.Loop = file.LoopOffset > 0

            file.LoopSamples = readBytes(fd, 0x20, 4).readInt32LE()

            file.Data = readBytes(
                fd,
                file.DataOffset,
                file.EoFOffset - file.DataOffset
            )

            // file.Data = file.Data.slice(0, 10)

            // let promise = Promise.resolve()

            // port.write(Buffer.from('ayte'))

            // return

            let offset = 0

            let loopOffset = file.LoopOffset - file.DataOffset

            let drained = true

            function writeBytes(offset, number) {
                return new Promise(resolve => {
                    // console.log(offset, number)
                    let data = file.Data.slice(offset, offset + number)
                    // console.log(offset, number, data)
                    // offset += number
                    // drained = false
                    // console.log(`Writing: ${data}`)
                    // data.push('\r')
                    // data.push('\n')
                    port.flush(_ => {
                        port.write(data)
                        port.drain(_ => {
                            // drained = true
                            console.log('drained')
                            resolve()
                        })
                    })
                    // port.flush(() => {
                    //     console.log('flushed')
                    //     // port.write(data)
                    //     // port.drain(() => {
                    //     //     console.log('drained')
                    //     //     // offset += number
                    //     //     drained = true
                    //     //     // resolve()
                    //     // })
                    // })
                })
            }

            let promise = Promise.resolve()

            let halt = false;

            // while (offset < file.Data.length) {
            //     //     if (halt) {
            //     //         continue
            //     //     }
            //     // halt = true
            //     // setInterval(_ => {
            //     let byte = file.Data[offset]
            //     switch (byte) {
            //         case 0x50: {
            //             let data = file.Data.slice(offset, offset + 2)
            //             port.flush(_ => {
            //                 port.write(data)
            //                 port.drain(_ => {
            //                     // drained = true
            //                     console.log('drained')
            //                     halt = false
            //                     // resolve()
            //                 })
            //             })
            //             break
            //         }
            //         default: {
            //             offset++
            //             console.log(`Unknown byte 0x${byte.toString(16)}`)
            //         }
            //     }
            //     // }, 1000)
            //     // console.log(offset, number, data)
            //     // offset += number
            //     // drained = false
            //     // console.log(`Writing: ${data}`)
            //     // data.push('\r')
            //     // data.push('\n')
            // }
            // while (offset < file.Data.length) {
            let totalWait = 0

            let maxOffset = false

            let wait;

            (function process() {
                // if (!drained) {
                //     return
                // }
                // setInterval(_ => {
                // if (offset >= file.Data.length) {
                //     return
                // }
                // console.log(offset, file.Data.length)
                // if (halt) {
                //     continue
                // }
                // drained = false
                wait = 0
                let byte = file.Data[offset]
                // console.log(`Byte 0x${byte.toString(16)}`)
                // promise = promise.then(_ => new Promise(resolve => {
                if (byte >= 0x30 && byte <= 0x3F) { // reserved // one operand
                    offset += 2
                } else if (byte >= 0x40 && byte <= 0x4F) { // reserved // two operands
                    offset += 3
                } else if (byte >= 0xA1 && byte <= 0xAF) { // reserved // two operands
                    offset += 3
                } else if (byte >= 0xC9 && byte <= 0xCF) { // reserved // three operands
                    offset += 4
                } else if (byte >= 0xD7 && byte <= 0xDF) { // reserved // three operands
                    offset += 4
                } else if (byte >= 0xE2 && byte <= 0xFF) { // reserved // four operands
                    offset += 5
                } else if (byte >= 0x70 && byte <= 0x7F) { // 0x7n // wait n+1 samples, n can range from 0 to 15
                    wait = (byte - 0x70 + 1) / 44100 * 1000
                    offset += 1
                } else {
                    switch (byte) {
                        // case 0x66: { // 0x66 // end of sound data
                        //     writeBytes(1)
                        //     break
                        // }
                        case 0x61: { // 0x61 nn nn // Wait n samples
                            let n = file.Data.slice(offset, offset + 3)
                                .readInt32LE(1, 2)
                            wait = n / 44100 * 1000
                            offset += 3
                            break
                        }
                        case 0x62: { // wait 735 samples (60th of a second)
                            wait = 735 / 44100 * 1000
                            offset += 1
                            break
                        }
                        case 0x63: { // wait 882 samples (50th of a second)
                            wait = 882 / 44100 * 1000
                            offset += 1
                            break
                        }
                        case 0x50: { // PSG (SN76489/SN76496) write value dd
                            // console.log(`PSG Write`);
                            // ((promise, offset) => {
                            //     promise = promise.then(_ => {
                            //         return writeBytes(offset, 2)
                            //     })
                            // })(promise, offset)
                            // await writeBytes(offset, 2)
                            let data = file.Data.slice(offset, offset + 2)
                            // console.log(data)
                            // console.log(offset, number, data)
                            // offset += number
                            // drained = false
                            // console.log(`Writing: ${data}`)
                            // data.push('\r')
                            // data.push('\n')
                            port.write(data)
                            // port.drain(() => {
                            //     drained = true
                            //     console.log('drained')
                            // })
                            offset += 2
                            // drained = false
                            // writeBytes(2).then(_ => {
                            //     console.log('drained')
                            //     drained = true
                            // })
                            break
                        }
                        default: { // skip invalid command
                            offset += 1
                            // console.log(`Unknown byte 0x${byte.toString(16)}`)
                        }
                        // }
                    }
                }

                (async () => {
                    if (offset < file.Data.length/* || file.LoopOffset > 0*/) {
                        // while (!drained) { }
                        if (wait > 0) {
                            totalWait += wait
                            await new Promise(
                                done => setTimeout(
                                    done,
                                    wait * (1 / speedMultiplier)
                                )
                            )
                            // setTimeout(() => {
                            //     process()
                            //     // port.drain(() => {
                            //     //     drained = true
                            //     //     console.log('drained')
                            //     // })
                            // }, nextWait)
                        }

                        process()

                        // console.log(totalWait, totalWait / 1000 * 44100, file.LoopSamples)

                        // if (offset > 2000) {
                        //     offset = file.LoopOffset - file.DataOffset
                        // }
                        // if (file.LoopOffset > 0 && offset >= file.Data.length - 1) {
                        //     offset = 0
                        // }
                    } else if (file.Loop &&
                        totalWait / 1000 * 44100 >= file.LoopSamples) {
                        offset = loopOffset
                        totalWait = 0
                        process()
                    }
                })()
                // halt = true;
                // (function (halt) {
                //     console.log(halt)
                //     setTimeout((halt) => {
                //         console.log(halt)
                //         halt = false
                //     }, 1000)
                // })(halt);
                // }))
                // offset++
            })()

            // process()
            // }, 1)

            // for (let offset = 0; offset < file.Data.length; offset++) {
            //     let byte = file.Data[offset]
            //     switch (byte) {
            //         case 0x61: { // 0x61 nn nn // Wait n samples
            //             break
            //         }
            //         case 0x66: { // 0x66 // end of sound data
            //             port.write()
            //             break
            //         }
            //         default: {
            //             continue
            //         }
            //     }
            //     break
            // }

            // let offset = 0;
            // let canRead = true;
            // while (true) {
            //     if (canRead) {
            //         canRead = false
            //         var buffer = Buffer.alloc(1)
            //         if (fs.readSync(fd, buffer, 0, 1, offset++) == 0) {
            //             break;
            //         }
            //         console.log(buffer)
            //         console.log(port.write(buffer))
            //         port.drain(() => {
            //             canRead = true
            //         })
            //     }
            // }

            console.log(file)

            // port.on('data', function (data) {
            //     console.log('Echo: ', data);
            // });
        })

        // port.write(buffer)

        // fs.open(argv.i, 'r', (status, fd) => {
        //     if (status) {
        //         console.error(status.message);
        //         return;
        //     }

        //     if (readBytes(fd, 0x00, 4).toString() != 'Vgm ') {
        //         console.error('File is not a vgm file')
        //         return;
        //     }

        //     port.write(fs.readFileSync(argv.i))

        //     // file.Version = readByte(fd, 0x09).toString(16) + '.' + readByte(fd, 0x08).toString(16)
        //     // file.SN76489Clock = readBytes(fd, 0x0C, 4).readInt32LE()
        //     // // VGM data offset is it's value + header
        //     // // address to compensate for relative value
        //     // file.DataOffset = readBytes(fd, 0x34, 4).readInt32LE() + 0x34

        //     // file.EoFOffset = readBytes(fd, 0x34, 4).readInt32LE() + 0x34
        //     // let offset = 0;
        //     // let canRead = true;
        //     // while (true) {
        //     //     if (canRead) {
        //     //         canRead = false
        //     //         var buffer = Buffer.alloc(1)
        //     //         if (fs.readSync(fd, buffer, 0, 1, offset++) == 0) {
        //     //             break;
        //     //         }
        //     //         console.log(buffer)
        //     //         console.log(port.write(buffer))
        //     //         port.drain(() => {
        //     //             canRead = true
        //     //         })
        //     //     }
        //     // }
        // })

        // setInterval(() => {
        //     // port.flush()

        //     port.write(Buffer.from('test'))
        //     // port.drain()
        // }, 1500)

    }, 2000)
})