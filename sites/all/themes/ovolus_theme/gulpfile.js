'use strict';

var gulp = require('gulp');
var sass = require('gulp-sass');
var sourcemaps = require('gulp-sourcemaps');
var autoprefixer = require('gulp-autoprefixer');
var browserSync = require('browser-sync').create();
var exec = require('child-process-promise').exec;
var logger = require("eazy-logger").Logger();

browserSync.init({
  socket: {
    domain: 'localhost:3000'
  }
});

gulp.task('mazeblock_theme:sass', function () {
  gulp.src('./sass/*.scss')
    .pipe(sourcemaps.init())
    .pipe(sass({
      includePaths: [
        './node_modules/breakpoint-sass/stylesheets/',
        './node_modules/compass-mixins/lib/',
        './node_modules/bootstrap-sass/assets/stylesheets/',
        './node_modules/font-awesome/scss/'
      ]
    }).on('error', sass.logError))
    .pipe(autoprefixer({
      browsers: ['last 2 version']
    }))
    .pipe(sourcemaps.write('.'))
    .pipe(gulp.dest('./css'))
    .pipe(browserSync.stream({match: '**/*.css'}));
});

gulp.task('mazeblock_theme:watch', function () {
  gulp.watch('./sass/**/*.scss', ['mazeblock_theme:sass']);
});

gulp.task('default', ['mazeblock_theme:sass', 'mazeblock_theme:watch']);