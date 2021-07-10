@extends('layouts.app')
@section('title', ($title ?? 'RinPhone') . ' | ' . config('app.name'))
@push('head.css')
    {{-- <link href="{{ mix('css/dashboard/app.css') }}" rel="stylesheet" id="app-css" /> --}}
@endpush
@section('body.content')
    {{-- <div id="app"></div> --}}
@endsection
@push('body.js')
    {!! ssr('/js/app-server.js')->render() !!}
    <script defer src="{{ mix('/js/app-client.js') }}"></script>
@endpush