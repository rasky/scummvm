/* ScummVM - Graphic Adventure Engine
 *
 * ScummVM is the legal property of its developers, whose names
 * are too numerous to list here. Please refer to the COPYRIGHT
 * file distributed with this source distribution.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.

 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.

 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA.
 *
 * $URL$
 * $Id$
 *
 */

#include "made/pmvplayer.h"

namespace Made {

PmvPlayer::PmvPlayer(OSystem *system, Audio::Mixer *mixer) : _fd(NULL), _system(system), _mixer(mixer) {
}

PmvPlayer::~PmvPlayer() {
}

void PmvPlayer::play(const char *filename) {

	_abort = false;
	_surface = NULL;

	_fd = new Common::File();
	_fd->open(filename);

	uint32 chunkType, chunkSize;

	readChunk(chunkType, chunkSize);	// "MOVE"
	readChunk(chunkType, chunkSize);	// "MHED"

	// TODO: Evaluate header
	//_fd->skip(0x3A);

	uint frameDelay = _fd->readUint16LE();
	_fd->skip(10);
	uint soundFreq = _fd->readUint16LE();
	// FIXME: weird frequencies... (11127 or 22254)
	//if (soundFreq == 11127) soundFreq = 11025;
	//if (soundFreq == 22254) soundFreq = 22050;

	int unk;

	for (int i = 0; i < 22; i++) {
		unk = _fd->readUint16LE();
		debug(2, "%i ", unk);
	}
	debug(2, "\n");

	_mixer->stopAll();

	// Read palette
	_fd->read(_palette, 768);
	updatePalette();

	uint32 frameCount = 0;
	uint16 chunkCount = 0;
	uint32 soundSize = 0;
	uint32 palChunkOfs = 0;
	uint32 palSize = 0;
	byte *frameData, *audioData, *soundData, *palData, *imageData;
	bool firstTime = true;

	uint32 frameNum;
	uint16 width, height, cmdOffs, pixelOffs, maskOffs, lineSize;

	// TODO: Sound can still be a little choppy. A bug in the decoder or -
	// perhaps more likely - do we have to implement double buffering to
	// get it to work well?
	_audioStream = Audio::makeAppendableAudioStream(soundFreq, Audio::Mixer::FLAG_UNSIGNED);

	while (!_abort && !_fd->eof()) {

		readChunk(chunkType, chunkSize);

		if (_fd->eof())
			break;

		frameData = new byte[chunkSize];
		_fd->read(frameData, chunkSize);

		// Handle audio
		audioData = frameData + READ_LE_UINT32(frameData + 8) - 8;
		chunkSize = READ_LE_UINT16(audioData + 4);
		chunkCount = READ_LE_UINT16(audioData + 6);

		if (chunkCount > 50) break;	// FIXME: this is a hack

		debug(2, "chunkCount = %d; chunkSize = %d\n", chunkCount, chunkSize);

		soundSize = chunkCount * chunkSize;
		soundData = new byte[soundSize];
		decompressSound(audioData + 8, soundData, chunkSize, chunkCount);
		_audioStream->queueBuffer(soundData, soundSize);

		// Handle palette
		palChunkOfs = READ_LE_UINT32(frameData + 16);
		if (palChunkOfs) {
			palData = frameData + palChunkOfs - 8;
			palSize = READ_LE_UINT32(palData + 4);
			decompressPalette(palData + 8, _palette, palSize);
		}

		// Handle video
		imageData = frameData + READ_LE_UINT32(frameData + 12) - 8;

		frameNum = READ_LE_UINT32(frameData);
		width = READ_LE_UINT16(imageData + 8);
		height = READ_LE_UINT16(imageData + 10);
		cmdOffs = READ_LE_UINT16(imageData + 12);
		pixelOffs = READ_LE_UINT16(imageData + 16);
		maskOffs = READ_LE_UINT16(imageData + 20);
		lineSize = READ_LE_UINT16(imageData + 24);

		debug(2, "width = %d; height = %d; cmdOffs = %04X; pixelOffs = %04X; maskOffs = %04X; lineSize = %d\n",
			width, height, cmdOffs, pixelOffs, maskOffs, lineSize);

		if (!_surface) {
			_surface = new Graphics::Surface();
			_surface->create(width, height, 1);
		}

		decompressImage(imageData, *_surface, cmdOffs, pixelOffs, maskOffs, lineSize, frameNum > 0);
	
		if (firstTime) {
			_mixer->playInputStream(Audio::Mixer::kPlainSoundType, &_audioStreamHandle, _audioStream);
			firstTime = false;
		}

		updatePalette();
		handleEvents();
		updateScreen();

		frameCount++;

		delete[] frameData;

		while (_mixer->getSoundElapsedTime(_audioStreamHandle) < frameCount * frameDelay) {
			_system->delayMillis(10);
		}

	}

	_audioStream->finish();
	_mixer->stopAll();
	
	//delete _audioStream;
	delete _fd;
	delete _surface;

}

void PmvPlayer::readChunk(uint32 &chunkType, uint32 &chunkSize) {
	chunkType = _fd->readUint32BE();
	chunkSize = _fd->readUint32LE();

	debug(2, "ofs = %08X; chunkType = %c%c%c%c; chunkSize = %d\n",
		_fd->pos(),
		(chunkType >> 24) & 0xFF, (chunkType >> 16) & 0xFF, (chunkType >> 8) & 0xFF, chunkType & 0xFF,
		chunkSize);

}

void PmvPlayer::handleEvents() {
	Common::Event event;
	while (_system->getEventManager()->pollEvent(event)) {
		switch (event.type) {
		case Common::EVENT_KEYDOWN:
			if (event.kbd.keycode == Common::KEYCODE_ESCAPE)
				_abort = true;
			break;
		case Common::EVENT_QUIT:
			// TODO: Exit more gracefully
			g_system->quit();
			break;
		default:
			break;
		}
	}
}

void PmvPlayer::updatePalette() {
	byte colors[1024];
	for (int i = 0; i < 256; i++) {
		colors[i * 4 + 0] = _palette[i * 3 + 0];
		colors[i * 4 + 1] = _palette[i * 3 + 1];
		colors[i * 4 + 2] = _palette[i * 3 + 2];
		colors[i * 4 + 3] = 0;
	}
	_system->setPalette(colors, 0, 256);
}

void PmvPlayer::updateScreen() {
	_system->copyRectToScreen((const byte*)_surface->pixels, _surface->pitch, 0, 0, _surface->w, _surface->h);
	_system->updateScreen();
}

void PmvPlayer::decompressPalette(byte *palData, byte *outPal, uint32 palDataSize) {
	byte *palDataEnd = palData + palDataSize;
	while (palData < palDataEnd) {
		byte count = *palData++;
		byte entry = *palData++;
		if (count == 255 && entry == 255)
			break;
		memcpy(&outPal[entry * 3], palData, (count + 1) * 3);
		palData += (count + 1) * 3;
	}
}

}
