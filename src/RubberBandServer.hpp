/*
 * Copyright(c) 2009 by Gabriel M. Beddingfield <gabriel@teuton.org>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY, without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 *
 */
#ifndef RUBBERBANDSERVER_HPP
#define RUBBERBANDSERVER_HPP

#include <stdint.h>
#include <memory>
#include "RingBuffer.hpp"
#include <QMutex>
#include <QWaitCondition>
#include <QThread>
#include <vector>

namespace RubberBand
{
    class RubberBandStretcher;
}

namespace StretchPlayer
{
    /**
     * \brief A RubberBandStretcher object contained in its own thread.
     *
     * This is designed for a stereo setup only.
     */
    class RubberBandServer : private QThread
    {
    public:
	typedef Tritium::RingBuffer<float> ringbuffer_t;

	RubberBandServer( uint32_t sample_rate );
	~RubberBandServer();

	void start();
	void shutdown();
	void wait();
	bool is_running();

	void reset();
	void time_ratio( float val );
	float time_ratio();
	void pitch_scale( float val );
	float pitch_scale();

	void go_idle();
	void go_active();

	void set_segment_size(unsigned long nframes);
	uint32_t feed_block_min() const;
	uint32_t feed_block_max() const;
	void nudge(); // Wake up thread in case it's sleeping.
	uint32_t latency() const;
	uint32_t written();
	uint32_t available_write();
	uint32_t write_audio(float* left, float* right, uint32_t count);
	uint32_t available_read();
	uint32_t read_audio(float* left, float* right, uint32_t count);
	float cpu_load() const;

    private:
	virtual void run();
	void _process();
	void _update_cpu_load();

    private:
	bool _running;
	std::auto_ptr< RubberBand::RubberBandStretcher > _stretcher;
	std::auto_ptr< ringbuffer_t > _inputs[2];
	std::auto_ptr< ringbuffer_t > _outputs[2];
	unsigned long _stretcher_feed_block;

	mutable QWaitCondition _wait_cond;
	mutable QMutex _wait_mutex;

	std::vector<uint32_t> _proc_time; // usecs
	std::vector<uint32_t> _idle_time; // usecs
	float _cpu_load; // [0.0, 1.0]

	mutable QMutex _param_mutex; // Must be locked for these params:
	float _time_ratio_param;
	float _pitch_scale_param;
	bool _reset_param;
    };

} // namespace StretchPlayer

#endif // RUBBERBANDSERVER_HPP
