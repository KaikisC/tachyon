// Copyright 2020-2022 The Electric Coin Company
// Copyright 2022 The Halo2 developers
// Use of this source code is governed by a MIT/Apache-2.0 style license that
// can be found in the LICENSE-MIT.halo2 and the LICENCE-APACHE.halo2
// file.

#include "tachyon/crypto/transcripts/blake2b_transcript.h"

#include <utility>
#include <vector>

#include "gtest/gtest.h"

#include "tachyon/math/elliptic_curves/bn/bn254/g1.h"

namespace tachyon::crypto {

namespace {

class Blake2bTranscriptTest : public testing::Test {
 public:
  static void SetUpTestSuite() { math::bn254::G1Curve::Init(); }
};

}  // namespace

TEST_F(Blake2bTranscriptTest, WritePoint) {
  using Curve = math::bn254::G1Curve;

  base::VectorBuffer write_buf;
  Blake2bWriter<Curve> writer(std::move(write_buf));
  Curve::AffinePointTy expected = Curve::AffinePointTy::Random();
  ASSERT_TRUE(writer.WriteToProof(expected));

  base::Buffer read_buf(writer.buffer().buffer(), writer.buffer().buffer_len());
  Blake2bReader<Curve> reader(std::move(read_buf));
  Curve::AffinePointTy actual;
  ASSERT_TRUE(reader.ReadPoint(&actual));

  EXPECT_EQ(expected, actual);
}

TEST_F(Blake2bTranscriptTest, WriteScalar) {
  using Curve = math::bn254::G1Curve;

  base::VectorBuffer write_buf;
  Blake2bWriter<Curve> writer(std::move(write_buf));
  Curve::ScalarField expected = Curve::ScalarField::Random();
  ASSERT_TRUE(writer.WriteToProof(expected));

  base::Buffer read_buf(writer.buffer().buffer(), writer.buffer().buffer_len());
  Blake2bReader<Curve> reader(std::move(read_buf));
  Curve::ScalarField actual;
  ASSERT_TRUE(reader.ReadScalar(&actual));

  EXPECT_EQ(expected, actual);
}

TEST_F(Blake2bTranscriptTest, SqueezeChallenge) {
  using Curve = math::bn254::G1Curve;

  base::VectorBuffer write_buf;
  Blake2bWriter<Curve> writer(std::move(write_buf));
  Curve::AffinePointTy generator = Curve::AffinePointTy::Generator();
  ASSERT_TRUE(writer.WriteToProof(generator));

  std::vector<uint8_t> expected_bytes = {26,  175, 9,   178, 100, 103, 8,   151,
                                         102, 161, 101, 61,  222, 95,  165, 141,
                                         175, 20,  97,  43,  214, 93,  71,  110,
                                         109, 175, 18,  100, 39,  250, 214, 25};
  Curve::ScalarField expected = Curve::ScalarField::FromBigInt(
      math::BigInt<4>::FromBytesLE(expected_bytes));

  Curve::ScalarField actual = writer.SqueezeChallenge().ChallengeAsScalar();

  EXPECT_EQ(expected, actual);
}

}  // namespace tachyon::crypto
